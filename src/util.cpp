// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <util.h>

#include <chainparamsbase.h>
#include <fs.h>
#include <random.h>
#include <serialize.h>
#include <sync.h>
#include <utilstrencodings.h>
#include <utiltime.h>

#include <stdarg.h>
#include <stdio.h>

#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__))
#include <pthread.h>
#include <pthread_np.h>
#endif

#ifndef WIN32
// for posix_fallocate
#ifdef __linux__

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#define _POSIX_C_SOURCE 200112L

#endif // __linux__

#include <algorithm>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>

#else

#ifdef _MSC_VER
#pragma warning(disable:4786)
#pragma warning(disable:4804)
#pragma warning(disable:4805)
#pragma warning(disable:4717)
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501

#ifdef _WIN32_IE
#undef _WIN32_IE
#endif
#define _WIN32_IE 0x0501

#define WIN32_LEAN_AND_MEAN 1
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <io.h> /* for _commit */
#include <shlobj.h>
#endif

#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include <boost/algorithm/string/case_conv.hpp> // for to_lower()
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp> // for startswith() and endswith()
#include <boost/foreach.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/thread.hpp>
#include <openssl/crypto.h>
#include <openssl/conf.h>
#include <thread>

// Work around clang compilation problem in Boost 1.46:
// /usr/include/boost/program_options/detail/config_file.hpp:163:17: error: call to function 'to_internal' that is neither visible in the template definition nor found by argument-dependent lookup
// See also: http://stackoverflow.com/questions/10020179/compilation-fail-in-boost-librairies-program-options
//           http://clang.debian.net/status.php?version=3.0&key=CANNOT_FIND_FUNCTION
namespace boost {

    namespace program_options {
        std::string to_internal(const std::string&);
    }

} // namespace boost



const char * const BITCOIN_CONF_FILENAME = "litecoinzd.conf";
const char * const BITCOIN_PID_FILENAME = "litecoinzd.pid";

std::map<std::string, std::string> mapArgs;
std::map<std::string, std::vector<std::string> > mapMultiArgs;
bool fPrintToConsole = false;
bool fPrintToDebugLog = true;
bool fQtGui = false;
bool fDaemon = false;
bool fServer = false;

bool fLogTimestamps = DEFAULT_LOGTIMESTAMPS;
bool fLogTimeMicros = DEFAULT_LOGTIMEMICROS;
bool fLogIPs = DEFAULT_LOGIPS;
std::atomic<bool> fReopenDebugLog(false);
CTranslationInterface translationInterface;

/** Log categories bitfield. Leveldb/libevent need special handling if their flags are changed at runtime. */
std::atomic<uint32_t> logCategories(0);

/** Init OpenSSL library multithreading support */
static CCriticalSection** ppmutexOpenSSL;
void locking_callback(int mode, int i, const char* file, int line) NO_THREAD_SAFETY_ANALYSIS
{
    if (mode & CRYPTO_LOCK) {
        ENTER_CRITICAL_SECTION(*ppmutexOpenSSL[i]);
    } else {
        LEAVE_CRITICAL_SECTION(*ppmutexOpenSSL[i]);
    }
}

// Init
static class CInit
{
public:
    CInit()
    {
        // Init OpenSSL library multithreading support
        ppmutexOpenSSL = (CCriticalSection**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(CCriticalSection*));
        for (int i = 0; i < CRYPTO_num_locks(); i++)
            ppmutexOpenSSL[i] = new CCriticalSection();
        CRYPTO_set_locking_callback(locking_callback);

        // OpenSSL can optionally load a config file which lists optional loadable modules and engines.
        // We don't use them so we don't require the config. However some of our libs may call functions
        // which attempt to load the config file, possibly resulting in an exit() or crash if it is missing
        // or corrupt. Explicitly tell OpenSSL not to try to load the file. The result for our libs will be
        // that the config appears to have been loaded and there are no modules/engines available.
        OPENSSL_no_config();
    }
    ~CInit()
    {
        // Shutdown OpenSSL library multithreading support
        CRYPTO_set_locking_callback(nullptr);
        for (int i = 0; i < CRYPTO_num_locks(); i++)
            delete ppmutexOpenSSL[i];
        OPENSSL_free(ppmutexOpenSSL);
    }
}
instance_of_cinit;

/**
 * LogPrintf() has been broken a couple of times now
 * by well-meaning people adding mutexes in the most straightforward way.
 * It breaks because it may be called by global destructors during shutdown.
 * Since the order of destruction of static/global objects is undefined,
 * defining a mutex as a global object doesn't work (the mutex gets
 * destroyed, and then some later destructor calls OutputDebugStringF,
 * maybe indirectly, and you get a core dump at shutdown trying to lock
 * the mutex).
 */

static boost::once_flag debugPrintInitFlag = BOOST_ONCE_INIT;

/**
 * We use boost::call_once() to make sure mutexDebugLog and
 * vMsgsBeforeOpenLog are initialized in a thread-safe manner.
 *
 * NOTE: fileout, mutexDebugLog and sometimes vMsgsBeforeOpenLog
 * are leaked on exit. This is ugly, but will be cleaned up by
 * the OS/libc. When the shutdown sequence is fully audited and
 * tested, explicit destruction of these objects can be implemented.
 */
static FILE* fileout = nullptr;
static boost::mutex* mutexDebugLog = nullptr;
static std::list<std::string> *vMsgsBeforeOpenLog;

[[noreturn]] void new_handler_terminate()
{
    // Rather than throwing std::bad-alloc if allocation fails, terminate
    // immediately to (try to) avoid chain corruption.
    // Since LogPrintf may itself allocate memory, set the handler directly
    // to terminate first.
    std::set_new_handler(std::terminate);
    fputs("Error: Out of memory. Terminating.\n", stderr);
    LogPrintf("Error: Out of memory. Terminating.\n");

    // The log was successful, terminate now.
    std::terminate();
};

static int FileWriteStr(const std::string &str, FILE *fp)
{
    return fwrite(str.data(), 1, str.size(), fp);
}

static void DebugPrintInit()
{
    assert(mutexDebugLog == nullptr);
    mutexDebugLog = new boost::mutex();
    vMsgsBeforeOpenLog = new std::list<std::string>;
}

void OpenDebugLog()
{
    boost::call_once(&DebugPrintInit, debugPrintInitFlag);
    boost::mutex::scoped_lock scoped_lock(*mutexDebugLog);

    assert(fileout == nullptr);
    assert(vMsgsBeforeOpenLog);
    fs::path pathDebug = GetDataDir() / "debug.log";
    fileout = fopen(pathDebug.string().c_str(), "a");
    if (fileout) setbuf(fileout, nullptr); // unbuffered

    // dump buffered messages from before we opened the log
    while (!vMsgsBeforeOpenLog->empty()) {
        FileWriteStr(vMsgsBeforeOpenLog->front(), fileout);
        vMsgsBeforeOpenLog->pop_front();
    }

    delete vMsgsBeforeOpenLog;
    vMsgsBeforeOpenLog = nullptr;
}

struct CLogCategoryDesc
{
    uint32_t flag;
    std::string category;
};

const CLogCategoryDesc LogCategories[] =
{
    {BCLog::NONE, "none"},
    {BCLog::NET, "net"},
    {BCLog::TOR, "tor"},
    {BCLog::MEMPOOL, "mempool"},
    {BCLog::HTTP, "http"},
    {BCLog::BENCH, "bench"},
    {BCLog::ZMQ, "zmq"},
    {BCLog::DB, "db"},
    {BCLog::RPC, "rpc"},
    {BCLog::ESTIMATEFEE, "estimatefee"},
    {BCLog::ADDRMAN, "addrman"},
    {BCLog::SELECTCOINS, "selectcoins"},
    {BCLog::REINDEX, "reindex"},
    {BCLog::CMPCTBLOCK, "cmpctblock"},
    {BCLog::RAND, "rand"},
    {BCLog::PRUNE, "prune"},
    {BCLog::PROXY, "proxy"},
    {BCLog::MEMPOOLREJ, "mempoolrej"},
    {BCLog::LIBEVENT, "libevent"},
    {BCLog::COINDB, "coindb"},
    {BCLog::QT, "qt"},
    {BCLog::LEVELDB, "leveldb"},
    {BCLog::POW, "pow"},
    {BCLog::ALERT, "alert"},
    {BCLog::PAYMENTDISCLOSURE, "paymentdisclosure"},
    {BCLog::ZRPCUNSAFE, "zrpcunsafe"},
    {BCLog::ZRPC, "zrpc"},
    {BCLog::ALL, "1"},
    {BCLog::ALL, "all"},
};

bool GetLogCategory(uint32_t *f, const std::string *str)
{
    if (f && str) {
        if (*str == "") {
            *f = BCLog::ALL;
            return true;
        }
        for (unsigned int i = 0; i < ARRAYLEN(LogCategories); i++) {
            if (LogCategories[i].category == *str) {
                *f = LogCategories[i].flag;
                return true;
            }
        }
    }
    return false;
}

std::string ListLogCategories()
{
    std::string ret;
    int outcount = 0;
    for (unsigned int i = 0; i < ARRAYLEN(LogCategories); i++) {
        // Omit the special cases.
        if (LogCategories[i].flag != BCLog::NONE && LogCategories[i].flag != BCLog::ALL) {
            if (outcount != 0) ret += ", ";
            ret += LogCategories[i].category;
            outcount++;
        }
    }
    return ret;
}

/**
 * fStartedNewLine is a state variable held by the calling context that will
 * suppress printing of the timestamp when multiple calls are made that don't
 * end in a newline. Initialize it to true, and hold it, in the calling context.
 */
static std::string LogTimestampStr(const std::string &str, bool *fStartedNewLine)
{
    std::string strStamped;

    if (!fLogTimestamps)
        return str;

    if (*fStartedNewLine) {
        int64_t nTimeMicros = GetTimeMicros();
        strStamped = FormatISO8601DateTime(nTimeMicros/1000000);
        if (fLogTimeMicros) {
            strStamped.pop_back();
            strStamped += strprintf(".%06dZ", nTimeMicros%1000000);
        }
        int64_t mocktime = GetMockTime();
        if (mocktime) {
            strStamped += " (mocktime: " + FormatISO8601DateTime(mocktime) + ")";
        }
        strStamped += ' ' + str;
    } else
        strStamped = str;

    if (!str.empty() && str[str.size()-1] == '\n')
        *fStartedNewLine = true;
    else
        *fStartedNewLine = false;

    return strStamped;
}

int LogPrintStr(const std::string &str)
{
    int ret = 0; // Returns total number of characters written
    static bool fStartedNewLine = true;
    if (fPrintToConsole)
    {
        // print to console
        ret = fwrite(str.data(), 1, str.size(), stdout);
        fflush(stdout);
    }
    else if (fPrintToDebugLog)
    {
        boost::call_once(&DebugPrintInit, debugPrintInitFlag);
        boost::mutex::scoped_lock scoped_lock(*mutexDebugLog);

        std::string strTimestamped = LogTimestampStr(str, &fStartedNewLine);

        // buffer if we haven't opened the log yet
        if (fileout == nullptr) {
            assert(vMsgsBeforeOpenLog);
            ret = strTimestamped.length();
            vMsgsBeforeOpenLog->push_back(strTimestamped);
        }
        else
        {
            // reopen the log file, if requested
            if (fReopenDebugLog) {
                fReopenDebugLog = false;
                fs::path pathDebug = GetDataDir() / "debug.log";
                if (freopen(pathDebug.string().c_str(),"a",fileout) != nullptr)
                    setbuf(fileout, nullptr); // unbuffered
            }

            ret = FileWriteStr(strTimestamped, fileout);
        }
    }
    return ret;
}

/** Interpret string as boolean, for argument parsing */
static bool InterpretBool(const std::string& strValue)
{
    if (strValue.empty())
        return true;
    return (atoi(strValue) != 0);
}

/** Turn -noX into -X=0 */
static void InterpretNegativeSetting(std::string& strKey, std::string& strValue)
{
    if (strKey.length()>3 && strKey[0]=='-' && strKey[1]=='n' && strKey[2]=='o')
    {
        strKey = "-" + strKey.substr(3);
        strValue = InterpretBool(strValue) ? "0" : "1";
    }
}

void ParseParameters(int argc, const char* const argv[])
{
    mapArgs.clear();
    mapMultiArgs.clear();

    for (int i = 1; i < argc; i++)
    {
        std::string str(argv[i]);
        std::string strValue;
        size_t is_index = str.find('=');
        if (is_index != std::string::npos)
        {
            strValue = str.substr(is_index+1);
            str = str.substr(0, is_index);
        }
#ifdef WIN32
        boost::to_lower(str);
        if (boost::algorithm::starts_with(str, "/"))
            str = "-" + str.substr(1);
#endif

        if (str[0] != '-')
            break;

        // Interpret --foo as -foo.
        // If both --foo and -foo are set, the last takes effect.
        if (str.length() > 1 && str[1] == '-')
            str = str.substr(1);
        InterpretNegativeSetting(str, strValue);

        mapArgs[str] = strValue;
        mapMultiArgs[str].push_back(strValue);
    }
}

std::string GetArg(const std::string& strArg, const std::string& strDefault)
{
    if (mapArgs.count(strArg))
        return mapArgs[strArg];
    return strDefault;
}

int64_t GetArg(const std::string& strArg, int64_t nDefault)
{
    if (mapArgs.count(strArg))
        return atoi64(mapArgs[strArg]);
    return nDefault;
}

bool GetBoolArg(const std::string& strArg, bool fDefault)
{
    if (mapArgs.count(strArg))
        return InterpretBool(mapArgs[strArg]);
    return fDefault;
}

bool SoftSetArg(const std::string& strArg, const std::string& strValue)
{
    if (mapArgs.count(strArg))
        return false;
    mapArgs[strArg] = strValue;
    return true;
}

bool SoftSetBoolArg(const std::string& strArg, bool fValue)
{
    if (fValue)
        return SoftSetArg(strArg, std::string("1"));
    else
        return SoftSetArg(strArg, std::string("0"));
}

static const int screenWidth = 79;
static const int optIndent = 2;
static const int msgIndent = 7;

std::string HelpMessageGroup(const std::string &message) {
    return std::string(message) + std::string("\n\n");
}

std::string HelpMessageOpt(const std::string &option, const std::string &message) {
    return std::string(optIndent,' ') + std::string(option) +
           std::string("\n") + std::string(msgIndent,' ') +
           FormatParagraph(message, screenWidth - msgIndent, msgIndent) +
           std::string("\n\n");
}

static std::string FormatException(const std::exception* pex, const char* pszThread)
{
#ifdef WIN32
    char pszModule[MAX_PATH] = "";
    GetModuleFileNameA(nullptr, pszModule, sizeof(pszModule));
#else
    const char* pszModule = "LitecoinZ";
#endif
    if (pex)
        return strprintf(
            "EXCEPTION: %s       \n%s       \n%s in %s       \n", typeid(*pex).name(), pex->what(), pszModule, pszThread);
    else
        return strprintf(
            "UNKNOWN EXCEPTION       \n%s in %s       \n", pszModule, pszThread);
}

void PrintExceptionContinue(const std::exception* pex, const char* pszThread)
{
    std::string message = FormatException(pex, pszThread);
    LogPrintf("\n\n************************\n%s\n", message);
    fprintf(stderr, "\n\n************************\n%s\n", message.c_str());
}

fs::path GetDefaultDataDir()
{
    namespace fs = fs;
    // Windows < Vista: C:\Documents and Settings\Username\Application Data\LitecoinZ
    // Windows >= Vista: C:\Users\Username\AppData\Roaming\LitecoinZ
    // Mac: ~/Library/Application Support/LitecoinZ
    // Unix: ~/.litecoinz
#ifdef WIN32
    // Windows
    return GetSpecialFolderPath(CSIDL_APPDATA) / "LitecoinZ";
#else
    fs::path pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == nullptr || strlen(pszHome) == 0)
        pathRet = fs::path("/");
    else
        pathRet = fs::path(pszHome);
#ifdef MAC_OSX
    // Mac
    pathRet /= "Library/Application Support";
    TryCreateDirectory(pathRet);
    return pathRet / "LitecoinZ";
#else
    // Unix
    return pathRet / ".litecoinz";
#endif
#endif
}

static fs::path pathCached;
static fs::path pathCachedNetSpecific;
static fs::path zc_paramsPathCached;
static CCriticalSection csPathCached;

static fs::path ZC_GetBaseParamsDir()
{
    // Copied from GetDefaultDataDir and adapter for litecoinz params.

    namespace fs = fs;
    // Windows < Vista: C:\Documents and Settings\Username\Application Data\LitecoinZParams
    // Windows >= Vista: C:\Users\Username\AppData\Roaming\LitecoinZParams
    // Mac: ~/Library/Application Support/LitecoinZParams
    // Unix: ~/.litecoinz-params
#ifdef WIN32
    // Windows
    return GetSpecialFolderPath(CSIDL_APPDATA) / "LitecoinZParams";
#else
    fs::path pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == nullptr || strlen(pszHome) == 0)
        pathRet = fs::path("/");
    else
        pathRet = fs::path(pszHome);
#ifdef MAC_OSX
    // Mac
    pathRet /= "Library/Application Support";
    TryCreateDirectory(pathRet);
    return pathRet / "LitecoinZParams";
#else
    // Unix
    return pathRet / ".litecoinz-params";
#endif
#endif
}

const fs::path &ZC_GetParamsDir()
{
    namespace fs = fs;

    LOCK(csPathCached); // Reuse the same lock as upstream.

    fs::path &path = zc_paramsPathCached;

    // This can be called during exceptions by LogPrintf(), so we cache the
    // value so we don't have to do memory allocations after that.
    if (!path.empty())
        return path;

    path = ZC_GetBaseParamsDir();

    return path;
}

// Return the user specified export directory.  Create directory if it doesn't exist.
// If user did not set option, return an empty path.
// If there is a filesystem problem, throw an exception.
const fs::path GetExportDir()
{
    namespace fs = fs;
    fs::path path;
    if (mapArgs.count("-exportdir")) {
        path = fs::system_complete(mapArgs["-exportdir"]);
        if (fs::exists(path) && !fs::is_directory(path)) {
            throw std::runtime_error(strprintf("The -exportdir '%s' already exists and is not a directory", path.string()));
        }
        if (!fs::exists(path) && !fs::create_directories(path)) {
            throw std::runtime_error(strprintf("Failed to create directory at -exportdir '%s'", path.string()));
        }
    }
    return path;
}


const fs::path &GetDataDir(bool fNetSpecific)
{
    namespace fs = fs;

    LOCK(csPathCached);

    fs::path &path = fNetSpecific ? pathCachedNetSpecific : pathCached;

    // This can be called during exceptions by LogPrintf(), so we cache the
    // value so we don't have to do memory allocations after that.
    if (!path.empty())
        return path;

    if (mapArgs.count("-datadir")) {
        path = fs::system_complete(mapArgs["-datadir"]);
        if (!fs::is_directory(path)) {
            path = "";
            return path;
        }
    } else {
        path = GetDefaultDataDir();
    }
    if (fNetSpecific)
        path /= BaseParams().DataDir();

    fs::create_directories(path);

    return path;
}

void ClearDatadirCache()
{
    pathCached = fs::path();
    pathCachedNetSpecific = fs::path();
}

fs::path GetConfigFile()
{
    fs::path pathConfigFile(GetArg("-conf", BITCOIN_CONF_FILENAME));
    if (!pathConfigFile.is_complete())
        pathConfigFile = GetDataDir(false) / pathConfigFile;

    return pathConfigFile;
}

void ReadConfigFile(std::map<std::string, std::string>& mapSettingsRet,
                    std::map<std::string, std::vector<std::string> >& mapMultiSettingsRet)
{
    fs::ifstream streamConfig(GetConfigFile());
    if (!streamConfig.good())
        return; // No litecoinz.conf file is OK

    std::set<std::string> setOptions;
    setOptions.insert("*");

    for (boost::program_options::detail::config_file_iterator it(streamConfig, setOptions), end; it != end; ++it)
    {
        // Don't overwrite existing settings so command line settings override litecoinz.conf
        std::string strKey = std::string("-") + it->string_key;
        std::string strValue = it->value[0];
        InterpretNegativeSetting(strKey, strValue);
        if (mapSettingsRet.count(strKey) == 0)
            mapSettingsRet[strKey] = strValue;
        mapMultiSettingsRet[strKey].push_back(strValue);
    }
    // If datadir is changed in .conf file:
    ClearDatadirCache();
}

#ifndef WIN32
fs::path GetPidFile()
{
    fs::path pathPidFile(GetArg("-pid", BITCOIN_PID_FILENAME));
    if (!pathPidFile.is_complete()) pathPidFile = GetDataDir() / pathPidFile;
    return pathPidFile;
}

void CreatePidFile(const fs::path &path, pid_t pid)
{
    FILE* file = fopen(path.string().c_str(), "w");
    if (file)
    {
        fprintf(file, "%d\n", pid);
        fclose(file);
    }
}
#endif

bool RenameOver(fs::path src, fs::path dest)
{
#ifdef WIN32
    return MoveFileExA(src.string().c_str(), dest.string().c_str(),
                       MOVEFILE_REPLACE_EXISTING) != 0;
#else
    int rc = std::rename(src.string().c_str(), dest.string().c_str());
    return (rc == 0);
#endif /* WIN32 */
}

/**
 * Ignores exceptions thrown by Boost's create_directory if the requested directory exists.
 * Specifically handles case where path p exists, but it wasn't possible for the user to
 * write to the parent directory.
 */
bool TryCreateDirectory(const fs::path& p)
{
    try
    {
        return fs::create_directory(p);
    } catch (const fs::filesystem_error&) {
        if (!fs::exists(p) || !fs::is_directory(p))
            throw;
    }

    // create_directory didn't create the directory, it had to have existed already
    return false;
}

void FileCommit(FILE *fileout)
{
    fflush(fileout); // harmless if redundantly called
#ifdef WIN32
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(fileout));
    FlushFileBuffers(hFile);
#else
    #if defined(__linux__) || defined(__NetBSD__)
    fdatasync(fileno(fileout));
    #elif defined(__APPLE__) && defined(F_FULLFSYNC)
    fcntl(fileno(fileout), F_FULLFSYNC, 0);
    #else
    fsync(fileno(fileout));
    #endif
#endif
}

bool TruncateFile(FILE *file, unsigned int length) {
#if defined(WIN32)
    return _chsize(_fileno(file), length) == 0;
#else
    return ftruncate(fileno(file), length) == 0;
#endif
}

/**
 * this function tries to raise the file descriptor limit to the requested number.
 * It returns the actual file descriptor limit (which may be more or less than nMinFD)
 */
int RaiseFileDescriptorLimit(int nMinFD) {
#if defined(WIN32)
    return 2048;
#else
    struct rlimit limitFD;
    if (getrlimit(RLIMIT_NOFILE, &limitFD) != -1) {
        if (limitFD.rlim_cur < (rlim_t)nMinFD) {
            limitFD.rlim_cur = nMinFD;
            if (limitFD.rlim_cur > limitFD.rlim_max)
                limitFD.rlim_cur = limitFD.rlim_max;
            setrlimit(RLIMIT_NOFILE, &limitFD);
            getrlimit(RLIMIT_NOFILE, &limitFD);
        }
        return limitFD.rlim_cur;
    }
    return nMinFD; // getrlimit failed, assume it's fine
#endif
}

/**
 * this function tries to make a particular range of a file allocated (corresponding to disk space)
 * it is advisory, and the range specified in the arguments will never contain live data
 */
void AllocateFileRange(FILE *file, unsigned int offset, unsigned int length) {
#if defined(WIN32)
    // Windows-specific version
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(file));
    LARGE_INTEGER nFileSize;
    int64_t nEndPos = (int64_t)offset + length;
    nFileSize.u.LowPart = nEndPos & 0xFFFFFFFF;
    nFileSize.u.HighPart = nEndPos >> 32;
    SetFilePointerEx(hFile, nFileSize, 0, FILE_BEGIN);
    SetEndOfFile(hFile);
#elif defined(MAC_OSX)
    // OSX specific version
    fstore_t fst;
    fst.fst_flags = F_ALLOCATECONTIG;
    fst.fst_posmode = F_PEOFPOSMODE;
    fst.fst_offset = 0;
    fst.fst_length = (off_t)offset + length;
    fst.fst_bytesalloc = 0;
    if (fcntl(fileno(file), F_PREALLOCATE, &fst) == -1) {
        fst.fst_flags = F_ALLOCATEALL;
        fcntl(fileno(file), F_PREALLOCATE, &fst);
    }
    ftruncate(fileno(file), fst.fst_length);
#elif defined(__linux__)
    // Version using posix_fallocate
    off_t nEndPos = (off_t)offset + length;
    posix_fallocate(fileno(file), 0, nEndPos);
#else
    // Fallback version
    // TODO: just write one byte per block
    static const char buf[65536] = {};
    fseek(file, offset, SEEK_SET);
    while (length > 0) {
        unsigned int now = 65536;
        if (length < now)
            now = length;
        fwrite(buf, 1, now, file); // allowed to fail; this function is advisory anyway
        length -= now;
    }
#endif
}

void ShrinkDebugFile()
{
    // Scroll debug.log if it's getting too big
    fs::path pathLog = GetDataDir() / "debug.log";
    FILE* file = fopen(pathLog.string().c_str(), "r");
    if (file && fs::file_size(pathLog) > 10 * 1000000)
    {
        // Restart the file with some of the end
        std::vector <char> vch(200000,0);
        fseek(file, -((long)vch.size()), SEEK_END);
        int nBytes = fread(begin_ptr(vch), 1, vch.size(), file);
        fclose(file);

        file = fopen(pathLog.string().c_str(), "w");
        if (file)
        {
            fwrite(begin_ptr(vch), 1, nBytes, file);
            fclose(file);
        }
    }
    else if (file != nullptr)
        fclose(file);
}

#ifdef WIN32
fs::path GetSpecialFolderPath(int nFolder, bool fCreate)
{
    namespace fs = fs;

    char pszPath[MAX_PATH] = "";

    if(SHGetSpecialFolderPathA(nullptr, pszPath, nFolder, fCreate))
    {
        return fs::path(pszPath);
    }

    LogPrintf("SHGetSpecialFolderPathA() failed, could not obtain requested path.\n");
    return fs::path("");
}
#endif

fs::path GetTempPath() {
#if BOOST_FILESYSTEM_VERSION == 3
    return fs::temp_directory_path();
#else
    // TODO: remove when we don't support filesystem v2 anymore
    fs::path path;
#ifdef WIN32
    char pszPath[MAX_PATH] = "";

    if (GetTempPathA(MAX_PATH, pszPath))
        path = fs::path(pszPath);
#else
    path = fs::path("/tmp");
#endif
    if (path.empty() || !fs::is_directory(path)) {
        LogPrintf("GetTempPath(): failed to find temp path\n");
        return fs::path("");
    }
    return path;
#endif
}

void runCommand(const std::string& strCommand)
{
    int nErr = ::system(strCommand.c_str());
    if (nErr)
        LogPrintf("runCommand error: system(%s) returned %d\n", strCommand, nErr);
}

void RenameThread(const char* name)
{
#if defined(PR_SET_NAME)
    // Only the first 15 characters are used (16 - NUL terminator)
    ::prctl(PR_SET_NAME, name, 0, 0, 0);
#elif (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__))
    pthread_set_name_np(pthread_self(), name);

#elif defined(MAC_OSX)
    pthread_setname_np(name);
#else
    // Prevent warnings for unused parameters...
    (void)name;
#endif
}

void SetupEnvironment()
{
    // On most POSIX systems (e.g. Linux, but not BSD) the environment's locale
    // may be invalid, in which case the "C" locale is used as fallback.
#if !defined(WIN32) && !defined(MAC_OSX) && !defined(__FreeBSD__) && !defined(__OpenBSD__)
    try {
        std::locale(""); // Raises a runtime error if current locale is invalid
    } catch (const std::runtime_error&) {
        setenv("LC_ALL", "C", 1);
    }
#endif
    // The path locale is lazy initialized and to avoid deinitialization errors
    // in multithreading environments, it is set explicitly by the main thread.
    // A dummy locale is used to extract the internal default locale, used by
    // fs::path, which is then used to explicitly imbue the path.
    std::locale loc = fs::path::imbue(std::locale::classic());
    fs::path::imbue(loc);
}

bool SetupNetworking()
{
#ifdef WIN32
    // Initialize Windows Sockets
    WSADATA wsadata;
    int ret = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (ret != NO_ERROR || LOBYTE(wsadata.wVersion ) != 2 || HIBYTE(wsadata.wVersion) != 2)
        return false;
#endif
    return true;
}

std::string LicenseInfo()
{
    return "\n" +
           FormatParagraph(strprintf(_("Copyright (C) 2009-%i The Bitcoin Core Developers"), COPYRIGHT_YEAR)) + "\n" +
           FormatParagraph(strprintf(_("Copyright (C) 2015-%i The Zcash Developers"), COPYRIGHT_YEAR)) + "\n" +
           FormatParagraph(strprintf(_("Copyright (C) 2017-%i The LitecoinZ Developers"), COPYRIGHT_YEAR)) + "\n" +
           "\n" +
           FormatParagraph(_("This is experimental software.")) + "\n" +
           "\n" +
           FormatParagraph(_("Donations for supporting developers are welcome:")) + "\n" +
           "\n" +
           FormatParagraph(_("- LTC: LgSbGGqru5LVHE3cWcgwZDLjr3WorGahr3")) + "\n" +
           FormatParagraph(_("- BTC: 1N2rQimKbeUQA8N2LU5vGopYQJmZsBM2d6")) + "\n" +
           FormatParagraph(_("- ZEC: t1T8hAYzLNwe2rLQpFMjx9z9FHFd6oQ86P9")) + "\n" +
           FormatParagraph(_("- BTG: GNkwW2uS4ed7cofz94J8PrTXJnZgmk5nr2")) + "\n" +
           FormatParagraph(_("- BTCZ: t1cmr2QUMCEBRhUVrZHsgC6DnrdzC2FyHz7")) + "\n" +
           "\n" +
           FormatParagraph(_("Distributed under the MIT software license, see the accompanying file COPYING or <http://www.opensource.org/licenses/mit-license.php>.")) + "\n" +
           "\n" +
           FormatParagraph(_("This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit <https://www.openssl.org/> and cryptographic software written by Eric Young.")) +
           "\n";
}

int GetNumCores()
{
    return std::thread::hardware_concurrency();
}

std::string CopyrightHolders()
{
    std::string strCopyrightHolders = _(COPYRIGHT_HOLDERS);
    if (strCopyrightHolders.find("%s") == strCopyrightHolders.npos)
        return strCopyrightHolders;
    return strprintf(strCopyrightHolders, _(PACKAGE_NAME));
}

package=fontconfig
$(package)_version=2.12.6
$(package)_download_path=http://www.freedesktop.org/software/fontconfig/release/
$(package)_file_name=$(package)-$($(package)_version).tar.bz2
$(package)_sha256_hash=cf0c30807d08f6a28ab46c61b8dbd55c97d2f292cf88f3a07d3384687f31f017
$(package)_dependencies=freetype expat

define $(package)_set_vars
  $(package)_config_opts=--disable-docs --disable-static
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

# 2.12.1 uses CHAR_WIDTH which is reserved and clashes with some glibc versions, but newer versions of fontconfig
# have broken makefiles which needlessly attempt to re-generate headers with gperf.
# Instead, change all uses of CHAR_WIDTH, and disable the rule that forces header re-generation.
# This can be removed once the upstream build is fixed.
define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

package=curl
$(package)_version=7.55.1
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_download_path=https://curl.haxx.se/download
$(package)_sha256_hash=09576ea5e66672648d83dce3af16d0cb294d4cba2b5d166ade39655c495f4a20
$(package)_dependencies=openssl

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --disable-ipv6 --disable-manual --disable-ldap --disable-ldaps --disable-rtsp --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-gopher --enable-http --enable-ftp --without-random --with-ssl
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

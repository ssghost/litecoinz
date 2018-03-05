package=curl
$(package)_version=7.58.0
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_download_path=https://curl.haxx.se/download
$(package)_sha256_hash=cc245bf9a1a42a45df491501d97d5593392a03f7b4f07b952793518d97666115
$(package)_dependencies=openssl

define $(package)_set_vars
  $(package)_config_opts=--disable-shared --with-ssl
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

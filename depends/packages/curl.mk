package=curl
$(package)_version=7.61.0
$(package)_file_name=$(package)-$($(package)_version).tar.gz
$(package)_download_path=https://curl.haxx.se/download
$(package)_sha256_hash=64141f0db4945268a21b490d58806b97c615d3d0c75bf8c335bbe0efd13b45b5
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

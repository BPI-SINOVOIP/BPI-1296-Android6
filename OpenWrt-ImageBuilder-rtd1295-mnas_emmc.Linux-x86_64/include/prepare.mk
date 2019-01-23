prepare: staging-install

staging-install:
	@( \
	mkdir -p staging_dir; \
	for part in host target toolchain; do \
	    [ -d staging_dir/$$part* ] || \
	    (cd staging_dir; tar --checkpoint=6000 -xf ../tools/staging_$$part.txz;) \
	done; \
	)

PACKAGES=ipk

packages: ipkg-make-index
	(cd $(PACKAGES) ; \
		../ipkg-make-index . > Packages ; \
		gzip -c9 Packages > Packages.gz \
	)


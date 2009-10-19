REPOSITORIES=optware   \
	     optware-d \

packages: ipkg-make-index.sh
	for dir in $(REPOSITORIES) ; do \
		(cd $${dir} ; \
			../ipkg-make-index.sh . > Packages ; \
			gzip -c9 Packages > Packages.gz \
		) \
	done

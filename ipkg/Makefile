REPOSITORIES=optware optware-d

all: $(REPOSITORIES)

repo-%: ipkg-make-index.sh
	./ipkg-make-index.sh $* > $*/Packages
	gzip -c9 $*/Packages > $*/Packages.gz

%:
	[ ! -d $* ] || $(MAKE) repo-$*

$(REPOSITORIES): dummy

.PHONY: all dummy

REPOSITORIES=optware optware-d optware-rtn

all: $(REPOSITORIES)

repo-%: ipkg-make-index.sh
	./ipkg-make-index.sh $*
	gzip -c9 $*/Packages >$*/Packages.gz

%:
	[ ! -d $* ] || $(MAKE) repo-$*

$(REPOSITORIES): dummy

.PHONY: all dummy

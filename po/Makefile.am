all: lang

lang: 
	for i in *.po ; do \
		l=`basename "$$i" .po`; \
		dir=../data/locales/$$l/LC_MESSAGES/; \
		test -d "$$dir" || mkdir -p "$$dir"; \
		msgfmt -c "$$i" -o "$$dir/libmwparser.mo"; \
	done



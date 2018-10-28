BASE=menschzukunft

epub:
	unix2dos ${BASE}.html
	chmod a+r ${BASE}.html
	zip -r ${BASE}-html.zip ${BASE}.html images

sr:
	sed 's/@/ /g' ${BASE}-nbsp.txt > \
	  ${BASE}-utf8.txt
	unix2dos ${BASE}-utf8.txt
	zip -r ${BASE}-sr.zip ${BASE}-utf8.txt

ppv:
	sed 's/@/ /g' ${BASE}-nbsp.txt > \
	  ${BASE}-utf8.txt
	cp ${BASE}-nbsp.txt.bin ${BASE}-utf8.txt.bin
	unix2dos ${BASE}-utf8.txt
	unix2dos ${BASE}.html
	zip -r ${BASE}-ppv.zip ${BASE}.html ${BASE}.html.bin \
	${BASE}-utf8.txt ${BASE}-utf8.txt.bin images

du:
	sed 's/@/ /g' ${BASE}-nbsp.txt > \
	  ${BASE}-utf8.txt
	chmod a+r ${BASE}.html
	cp ${BASE}-nbsp.txt.bin ${BASE}-utf8.txt.bin
	unix2dos ${BASE}-utf8.txt
	unix2dos ${BASE}.html
	rm -f ${BASE}-du.zip
	zip -r ${BASE}-du.zip ${BASE}.html \
	${BASE}-utf8.txt images


upload:
	ssh poenisch@user.in-chemnitz.de \
	  "mkdir -p xml2lms.in-chemnitz.de/dp/${BASE}"
	scp ${BASE}-sr.zip ${BASE}-html.zip ${BASE}.mobi ${BASE}.epub \
	  poenisch@user.in-chemnitz.de:xml2lms.in-chemnitz.de/dp/${BASE}
.PHONY: sr

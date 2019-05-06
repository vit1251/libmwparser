#!/bin/sh

SITES="commons.wikimedia.org en
en.wikipedia.org en
en.wikibooks.org en
en.wikiversity.org en
en.wikisource.org en
fr.wikipedia.org fr
fr.wikibooks.org fr
fr.wikiversity.org fr
fr.wikisource.org fr
meta.wikimedia.org en
www.wikimedia.org en"

while read site lang
do
  for css in MediaWiki:Common.css MediaWiki:Monobook.css ; do
    curl -s "http://localhost:8888/?wikimedia=$site&lang=$lang&entry=$css&raw=1" |\
      grep "url(" | grep -v "css" | \
      sed -r 's/.*url\(['\''"]?(http[^)"'\'']*)['\''")].*/\1/' | \
      wget -c -N -x -i -
  done
done <<EOF
$SITES
EOF


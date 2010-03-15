#!/bin/bash

if [ "$1" = "--help" ]; then
	echo "Usage: `basename $0` [theme]"
	THEMES=`cd themes; ls`
	echo "theme can be one of the followings:"
	echo "${THEMES}"
	exit 0
fi

if [ -z "$1" ]; then
	DEFAULT_THEME=softy
else
	DEFAULT_THEME="$1"
fi

echo "Using theme ${DEFAULT_THEME} as default theme"

rm -rf site
mkdir -p site
mkdir -p site/images
mkdir -p site/downloads


OVERALL_NEWS=content/index/news.html
WHATS_NEW=template/whats_new.html
NEWS_LIST=`cd news; ls *.html`

declare -a MONTH=("January" "February" "March" "April" "May" "June" "July" "August" "September" "October" "November" "December")

echo "Building ${OVERALL_NEWS} and ${WHATS_NEW}..."

printf "" > ${OVERALL_NEWS}

for NEWS in ${NEWS_LIST}; do
	printf "\t<tr>\n" >> ${OVERALL_NEWS}
	printf "\t\t<td class=\"news-date\">\n" >> ${OVERALL_NEWS}
	YEAR=`printf ${NEWS} | cut -f 1 -d -`
	MONTH_NUMBER=`printf ${NEWS} | cut -f 2 -d - | sed 's/0*//'`
	MONTH_NUMBER=$((${MONTH_NUMBER} - 1))
	MONTH_NAME=${MONTH[${MONTH_NUMBER}]}
	DAY_NUMBER=`printf ${NEWS} | cut -f 3 -d - | cut -f 1 -d .`
	printf "\t\t\t${MONTH_NAME} ${DAY_NUMBER}, ${YEAR}\n" >> ${OVERALL_NEWS}
	printf "\t\t</td>\n" >> ${OVERALL_NEWS}
	printf "\t\t<td class=\"news-description\">\n" >> ${OVERALL_NEWS}
	printf "\t\t\t" >> ${OVERALL_NEWS}
	cat news/${NEWS} >> ${OVERALL_NEWS}
	printf "\n\t\t</td>\n" >> ${OVERALL_NEWS}
	printf "\t</tr>\n" >> ${OVERALL_NEWS}
	cat news/${NEWS} > ${WHATS_NEW}
done


THEMES=`cd themes; ls`

for THEME in ${THEMES}; do
	if [ -d "themes/${THEME}" ]; then
		if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
			THEME_ROOT=
		else
			THEME_ROOT="${THEME}"
			mkdir -p site/${THEME}
		fi

		mkdir -p site/${THEME_ROOT}/style

		THEME_STYLE=
		CONTENT_STYLE=

		if [ -f "themes/${THEME}/theme.css" ]; then
			echo "Copying stylesheet for theme ${THEME}"
			cp "themes/${THEME}/theme.css" "site/${THEME_ROOT}/style/theme.css" || exit 1
			THEME_STYLE="<link rel=\"stylesheet\" href=\"style/theme.css\" type=\"text/css\">"
		fi

		cp themes/${THEME}/*.png site/${THEME_ROOT}/style

		CONTENTS=`cd content; ls */*.html`

		for CONTENT in ${CONTENTS}; do
			CONTENT_DIR=`dirname ${CONTENT}`

			if [ -f "content/${CONTENT_DIR}/style.css" ]; then
				echo "Copying stylesheet for ${CONTENT_DIR}.html..."
				mkdir -p "site/${THEME_ROOT}/style/${CONTENT_DIR}"
				cp "content/${CONTENT_DIR}/style.css" "site/${THEME_ROOT}/style/${CONTENT_DIR}/style.css" || exit 1
				CONTENT_STYLE="<link rel=\"stylesheet\" href=\"style/${CONTENT_DIR}/style.css\" type=\"text/css\">"
			fi

			echo "Building ${CONTENT_DIR}.html for theme ${THEME}..."

			THEME_NAV=
			for THEME2 in ${THEMES}; do
				if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
					if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
						THEME_NAV="${THEME_NAV} <a href=\"${CONTENT_DIR}.html\">${THEME2}</a>"
					else
						THEME_NAV="${THEME_NAV} <a href=\"${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
					fi
				else
					if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
						THEME_NAV="${THEME_NAV} <a href=\"../${CONTENT_DIR}.html\">${THEME2}</a>"
					else
						THEME_NAV="${THEME_NAV} <a href=\"../${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
					fi
				fi
			done

			if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
				SITE_PREFIX=
			else
				SITE_PREFIX="../"
			fi

			PAGE_TITLE=`cat content/${CONTENT_DIR}/title.txt`
			
			cpp \
				"-DPAGE_TITLE=${PAGE_TITLE}" \
				"-DBASE_STYLE=<link rel=\"stylesheet\" href=\"${SITE_PREFIX}style/base.css\" type=\"text/css\">" \
				"-DTHEME_STYLE=${THEME_STYLE}" \
				"-DCONTENT_STYLE=${CONTENT_STYLE}" \
				"-DTHEME_NAV=${THEME_NAV}" \
				"-DIMAGES=${SITE_PREFIX}images" \
				"-DDOWNLOADS=${SITE_PREFIX}downloads" \
				-P \
				-Itemplate \
				-Icontent/${CONTENT_DIR} \
				template/template.html | sed "s/\`/\'/g"> site/${THEME_ROOT}/${CONTENT_DIR}.html || exit -1
		done
	fi
done

echo "Copying base stylesheet..."
cp template/base.css site/style/base.css || exit 1

IMAGES=`cd images; find . \( -name "*.png" -o -name "*.ico" \)`

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying images/${IMAGE}"
		mkdir -p site/images/`dirname "${IMAGE}"`
		cp "images/${IMAGE}" "site/images/${IMAGE}"
	fi
done

DOWNLOADS=`cd downloads; find . \( -name "*.tar.gz" -o -name "*.tar.bz2" -o -name "*.zip" -o -name "*.exe" -o -name "*.pdf" \)`

for DOWNLOAD in ${DOWNLOAD}; do
	if [ -f "downloads/${DOWNLOAD}" ]; then
		echo "Copying downloads/${DOWNLOAD}"
		mkdir -p site/download/`dirname "${DOWNLOAD}"`
		cp "downloads/${DOWNLOAD}" "site/downloads/${DOWNLOAD}"
	fi
done

echo "The web site is in ${PWD}/site"

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
mkdir -p site/thumbs
mkdir -p site/glyph
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

GLYPHES=`cd glyph; ls *.png *.ico`

for GLYPH in ${GLYPHES}; do
	if [ -f "glyph/${GLYPH}" ]; then
		echo "Copying glyph/${GLYPH}"
		cp "glyph/${GLYPH}" "site/glyph/${GLYPH}"
	fi
done

IMAGES=`cd images; ls *.png`

GALLERY=content/gallery/gallery.html

printf "" > ${GALLERY}

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying images/${IMAGE}"
		cp "images/${IMAGE}" "site/images/${IMAGE}"
		convert -thumbnail 158 "site/images/${IMAGE}" "site/thumbs/${IMAGE}"
		printf "<a href=\`\`IMAGES/${IMAGE}\`\`><img src=\`\`THUMBS/${IMAGE}\`\` alt=\"${IMAGE} thumbnail\"></a>" >> ${GALLERY}
	fi
done

IMAGES_GALLERY=content/gallery/gallery.html
IMAGES_GALLERY_COL=1

echo "Generating image gallery in ${IMAGE_GALLERY}..."

printf "<table class=\"images-gallery\">\n" > ${IMAGES_GALLERY}

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying images/${IMAGE}"
		mkdir -p site/images/`dirname "${IMAGE}"`
		cp "images/${IMAGE}" "site/images/${IMAGE}"

		echo "Generating gallery for image ${IMAGE}..."
		IMAGE_NAME=`echo ${IMAGE} | sed -e 's/\..*$//g'`
		CONTENT_DIR=content/gallery-${IMAGE_NAME}
 		mkdir -p "${CONTENT_DIR}"
		printf "Screenshot [${IMAGE}]" > "${CONTENT_DIR}/title.txt"
 		printf "\t<div class=\"image\">" > "${CONTENT_DIR}/content.html"
 		printf "\t\t\t<a href=\"gallery.html\"><img src=\`\`IMAGES/${IMAGE}\`\` alt=\"${IMAGE}\"></a>\n" >> "${CONTENT_DIR}/content.html"
 		printf "\t\t</div>" >> "${CONTENT_DIR}/content.html"

		printf ".aside\n" > "${CONTENT_DIR}/style.css"
		printf "{\n" >> "${CONTENT_DIR}/style.css"
		printf "\tdisplay:none;\n" >> "${CONTENT_DIR}/style.css"
		printf "}\n" >> "${CONTENT_DIR}/style.css"

		echo "Generating thumbnail for image ${IMAGE}..."
		convert -thumbnail 158 "site/images/${IMAGE}" "site/thumbs/${IMAGE}"
		if [ ${IMAGES_GALLERY_COL} -eq 1 ]; then
			printf "\t<tr>\n" >> ${IMAGES_GALLERY}
		fi
		printf "\t\t<td>\n" >> ${IMAGES_GALLERY} 
		printf "\t\t\t<a href=\"gallery-${IMAGE_NAME}.html\"><img src=\`\`THUMBS/${IMAGE}\`\` alt=\"${IMAGE} thumbnail\"></a>\n" >> ${IMAGES_GALLERY}
		printf "\t\t\t<p>${IMAGE}</p>\n" >> ${IMAGES_GALLERY}
		printf "\t\t</td>\n" >> ${IMAGES_GALLERY} 
		IMAGES_GALLERY_COL=$((${IMAGES_GALLERY_COL} + 1))
		if [ ${IMAGES_GALLERY_COL} -gt 3 ]; then
			IMAGES_GALLERY_COL=1
			printf "\t\t</tr>\n" >> ${IMAGES_GALLERY} 
		fi
	fi
done

if ! [ ${IMAGES_GALLERY_COL} -eq 1 ]; then
	if [ ${IMAGES_GALLERY_COL} -le 3 ]; then
		printf "\t\t</tr>\n" >> ${IMAGES_GALLERY} 
	fi
fi
printf "</table>\n" >> ${IMAGES_GALLERY}


DOWNLOADS=`cd downloads; find . \( -name "*.tar.gz" -o -name "*.tar.bz2" -o -name "*.zip" -o -name "*.exe" -o -name "*.pdf" \)`

for DOWNLOAD in ${DOWNLOAD}; do
	if [ -f "downloads/${DOWNLOAD}" ]; then
		echo "Copying downloads/${DOWNLOAD}"
		mkdir -p site/download/`dirname "${DOWNLOAD}"`
		cp "downloads/${DOWNLOAD}" "site/downloads/${DOWNLOAD}"
	fi
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
				"-DGLYPH=${SITE_PREFIX}glyph" \
				"-DTHUMBS=${SITE_PREFIX}thumbs" \
				"-DDOWNLOADS=${SITE_PREFIX}downloads" \
				-P \
				-Itemplate \
				-Icontent/${CONTENT_DIR} \
				template/template.html | sed -e "s/\`\`/\"/g" -e "s/\`/\'/g" > site/${THEME_ROOT}/${CONTENT_DIR}.html || exit -1
		done
	fi
done

echo "Copying base stylesheet..."
cp template/base.css site/style/base.css || exit 1

echo "Installing .htaccess..."
echo "deny from all" > site/.htaccess

echo "The web site is in ${PWD}/site"

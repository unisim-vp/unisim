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

# clean everything before starting
rm -rf site
rm -rf content/*-view-*

# create web site directory structure
mkdir -p site
mkdir -p site/style
mkdir -p site/images
mkdir -p site/thumbs
mkdir -p site/glyph
mkdir -p site/downloads

###############################################################################
#                                     NEWS                                    #
###############################################################################

MAX_HOME_NEWS=10

# list all materials
NEWS_LIST=`cd news; ls -r *.html`

# define generated files
HOME_NEWS=content/index/news.html
ALL_NEWS=content/news/news.html
WHATS_NEW=template/whats_new.html
rm -f ${HOME_NEWS}
rm -f ${ALL_NEWS}
rm -f ${WHATS_NEW}

declare -a MONTH=("January" "February" "March" "April" "May" "June" "July" "August" "September" "October" "November" "December")

echo "Building ${ALL_NEWS} and ${WHATS_NEW}..."

printf "" > ${ALL_NEWS}

NUM_NEWS=0
for NEWS in ${NEWS_LIST}; do
	printf '\t<tr>\n' >> ${ALL_NEWS}
	printf '\t\t<td class="news-date">\n' >> ${ALL_NEWS}
	YEAR=`printf ${NEWS} | cut -f 1 -d -`
	MONTH_NUMBER=`printf ${NEWS} | cut -f 2 -d - | sed 's/0*//'`
	MONTH_NUMBER=$((${MONTH_NUMBER} - 1))
	MONTH_NAME=${MONTH[${MONTH_NUMBER}]}
	DAY_NUMBER=`printf ${NEWS} | cut -f 3 -d - | cut -f 1 -d .`
	printf "\t\t\t${MONTH_NAME} ${DAY_NUMBER}, ${YEAR}\n" >> ${ALL_NEWS}
	printf "\t\t</td>\n" >> ${ALL_NEWS}
	printf "\t\t<td class=\"news-description\">\n" >> ${ALL_NEWS}
	printf "\t\t\t" >> ${ALL_NEWS}
	cat news/${NEWS} >> ${ALL_NEWS}
	printf "\n\t\t</td>\n" >> ${ALL_NEWS}
	printf "\t</tr>\n" >> ${ALL_NEWS}
	if ! [ -f ${WHATS_NEW} ]; then
		cp news/${NEWS} ${WHATS_NEW}
	fi
	NUM_NEWS=$((${NUM_NEWS} + 1))
	if [ ${NUM_NEWS} -eq ${MAX_HOME_NEWS} ]; then
		cp ${ALL_NEWS} ${HOME_NEWS}
	fi
done

if [ ${NUM_NEWS} -lt ${MAX_HOME_NEWS} ]; then
	cp ${ALL_NEWS} ${HOME_NEWS}
fi

###############################################################################
#                                    Site map                                 #
###############################################################################

CONTENTS=`cd content; ls`
SITE_MAP=content/sitemap/sitemap.html

printf "" > ${SITE_MAP}
for CONTENT_DIR in ${CONTENTS}; do
	CONTENT_TITLE="`cat content/${CONTENT_DIR}/title.txt`"
	if ! [ "${CONTENT_DIR}" = "sitemap" ]; then
		printf "<p><a class=\"online-document\" href=${CONTENT_DIR}.html>${CONTENT_TITLE}</a></p>\n" >> ${SITE_MAP}
	fi
done

###############################################################################
#                                    GLYPHES                                  #
###############################################################################

GLYPHES=`cd glyph; ls *.png *.ico`

for GLYPH in ${GLYPHES}; do
	if [ -f "glyph/${GLYPH}" ]; then
		echo "Copying glyph/${GLYPH}"
		cp "glyph/${GLYPH}" "site/glyph/${GLYPH}"
	fi
done

###############################################################################
#                                   IMAGES                                    #
###############################################################################

# list all materials
IMAGES=`cd images; ls *.png`

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying images/${IMAGE}"
		mkdir -p site/images/`dirname "${IMAGE}"`
		cp "images/${IMAGE}" "site/images/${IMAGE}"
	fi
done

###############################################################################
#                                    THUMBS                                   #
###############################################################################

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Generating thumbnail for image ${IMAGE}..."
		convert -thumbnail 256 "images/${IMAGE}" "site/thumbs/${IMAGE}"
	fi
done

###############################################################################
#                                    GALLERY                                  #
###############################################################################

GALLERY=content/gallery/gallery.html
rm -f ${GALLERY}

echo "Generating image gallery in ${GALLERY}..."

printf "" > ${GALLERY}
printf "<table class=\"gallery\">\n" > ${GALLERY}

GALLERY_COL=1
for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Adding image ${IMAGE} to gallery..."
		IMAGE_NAME=`echo ${IMAGE} | sed -e 's/\..*$//g'`

		if [ ${GALLERY_COL} -eq 1 ]; then
			printf "\t<tr>\n" >> ${GALLERY}
		fi
		printf "\t\t<td>\n" >> ${GALLERY} 
		printf "\t\t\t<a href=\"gallery-view-${IMAGE_NAME}.html\"><img src=\`\`THUMBS/${IMAGE}\`\` alt=\"${IMAGE} thumbnail\"></a>\n" >> ${GALLERY}
		printf "\t\t</td>\n" >> ${GALLERY} 
		GALLERY_COL=$((${GALLERY_COL} + 1))
		if [ ${GALLERY_COL} -gt 3 ]; then
			GALLERY_COL=1
			printf "\t</tr>\n" >> ${GALLERY} 
		fi
	fi
done

if ! [ ${GALLERY_COL} -eq 1 ]; then
	if [ ${GALLERY_COL} -le 3 ]; then
		printf "\t</tr>\n" >> ${GALLERY} 
	fi
fi
printf "</table>\n" >> ${GALLERY}

###############################################################################
#                                 IMAGE VIEW                                  #
###############################################################################

rm -rf content/*-view-*

CONTENTS=`cd content; ls`
for CONTENT_DIR in ${CONTENTS}; do
	CONTENT_TITLE="`cat content/${CONTENT_DIR}/title.txt`"
	IMAGES=`cat content/${CONTENT_DIR}/*.html | sed -n 's/.*src=\`\`.*\/\(.*\)\`\`.*/\1/Ip' | sort -u`

	for IMAGE in ${IMAGES}; do
		if [ -f "images/${IMAGE}" ]; then
			echo "Generating view for image ${IMAGE}..."
			IMAGE_NAME=`echo ${IMAGE} | sed -e 's/\..*$//g'`
			VIEW_DIR=content/${CONTENT_DIR}-view-${IMAGE_NAME}
			mkdir -p "${VIEW_DIR}"
			printf "<div class=\"image-view\">\n" > "${VIEW_DIR}/content.html"
			if [ -f "images/${IMAGE_NAME}.txt" ]; then
				printf "<h1>" >> "${VIEW_DIR}/content.html"
				cat "images/${IMAGE_NAME}.txt" >> "${VIEW_DIR}/content.html"
				printf "</h1>\n" >> "${VIEW_DIR}/content.html"
				cp "images/${IMAGE_NAME}.txt" "${VIEW_DIR}/title.txt"
			else
				printf "<h1>${IMAGE}</h1>\n" >> "${VIEW_DIR}/content.html"
				printf "${IMAGE}" > "${VIEW_DIR}/title.txt"
			fi
			printf "<table>\n" >> "${VIEW_DIR}/content.html"
			printf "\t<tr>\n" >> "${VIEW_DIR}/content.html"
			printf "\t\t<td>Download <a class=\"download-file\" href=\`\`IMAGES/${IMAGE}\`\`>${IMAGE}</a></td>\n" >> "${VIEW_DIR}/content.html"
			printf "\t\t<td>Enter <a class=\"online-document\" href=\"gallery.html\">Gallery</a></td>\n" >> "${VIEW_DIR}/content.html"
			printf "\t\t<td>Back to <a class=\"online-document\" href=\"${CONTENT_DIR}.html\">${CONTENT_TITLE}</a></td>\n" >> "${VIEW_DIR}/content.html"
			printf "\t</tr>\n" >> "${VIEW_DIR}/content.html"
			printf "</table>\n" >> "${VIEW_DIR}/content.html"
			printf "\t<a href=\"${CONTENT_DIR}.html\"><img src=\`\`IMAGES/${IMAGE}\`\` alt=\"${IMAGE}\"></a>\n" >> "${VIEW_DIR}/content.html"
			printf "\t</div>\n" >> "${VIEW_DIR}/content.html"

			printf ".aside\n" > "${VIEW_DIR}/style.css"
			printf "{\n" >> "${VIEW_DIR}/style.css"
			printf "\tdisplay:none;\n" >> "${VIEW_DIR}/style.css"
			printf "}\n" >> "${VIEW_DIR}/style.css"
		fi
	done
done

###############################################################################
#                                DOWNLOADS                                    #
###############################################################################

DOWNLOADS=`cd downloads; find . \( -name "*.tar.gz" -o -name "*.tar.bz2" -o -name "*.zip" -o -name "*.exe" -o -name "*.pdf" \)`

for DOWNLOAD in ${DOWNLOAD}; do
	if [ -f "downloads/${DOWNLOAD}" ]; then
		echo "Copying downloads/${DOWNLOAD}"
		mkdir -p site/download/`dirname "${DOWNLOAD}"`
		cp "downloads/${DOWNLOAD}" "site/downloads/${DOWNLOAD}"
	fi
done

###############################################################################
#                           CONTENT and STYLESHEET                            #
###############################################################################

# list all materials
THEMES=`cd themes; ls`
CONTENTS=`cd content; ls`

echo "Copying base stylesheet..."
cp template/base.css site/style/base.css || exit 1

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
			THEME_STYLE="style/theme.css"
		fi

		cp themes/${THEME}/*.png site/${THEME_ROOT}/style

		for CONTENT_DIR in ${CONTENTS}; do
			if [ -f "content/${CONTENT_DIR}/style.css" ]; then
				echo "Copying stylesheet for ${CONTENT_DIR}.html..."
				mkdir -p "site/${THEME_ROOT}/style/${CONTENT_DIR}"
				cp "content/${CONTENT_DIR}/style.css" "site/${THEME_ROOT}/style/${CONTENT_DIR}/style.css" || exit 1
				CONTENT_STYLE="style/${CONTENT_DIR}/style.css"
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

			PAGE_TITLE="`cat content/${CONTENT_DIR}/title.txt`"

			if [ -f "content/${CONTENT_DIR}/description.txt" ]; then
				META_DESCRIPTION="`cat content/${CONTENT_DIR}/description.txt`"
			else
				META_DESCRIPTION=
			fi

			if [ -f "content/${CONTENT_DIR}/keywords.txt" ]; then
				META_KEYWORDS="`cat content/${CONTENT_DIR}/keywords.txt`"
			else
				META_KEYWORDS=
			fi

			BASE_STYLE="${SITE_PREFIX}style/base.css"

			if ! [ -z PAGE_TITLE ]; then
				HAVE_PAGE_TITLE=1
			else
				HAVE_PAGE_TITLE=0
			fi

			if ! [ -z META_DESCRIPTION ]; then
				HAVE_META_DESCRIPTION=1
			else
				HAVE_META_DESCRIPTION=0
			fi

			if ! [ -z META_KEYWORDS ]; then
				HAVE_META_KEYWORDS=1
			else
				HAVE_META_KEYWORDS=0
			fi

			if ! [ -z BASE_STYLE ]; then
				HAVE_BASE_STYLE=1
			else
				HAVE_BASE_STYLE=0
			fi

			if ! [ -z THEME_STYLE ]; then
				HAVE_THEME_STYLE=1
			else
				HAVE_THEME_STYLE=0
			fi

			if ! [ -z CONTENT_STYLE ]; then
				HAVE_CONTENT_STYLE=1
			else
				HAVE_CONTENT_STYLE=0
			fi

			cpp \
				"-DHAVE_PAGE_TITLE=${HAVE_PAGE_TITLE}" \
				"-DPAGE_TITLE=${PAGE_TITLE}" \
				"-DHAVE_META_DESCRIPTION=${HAVE_META_DESCRIPTION}" \
				"-DMETA_DESCRIPTION=${META_DESCRIPTION}" \
				"-DHAVE_META_KEYWORDS=${HAVE_META_KEYWORDS}" \
				"-DMETA_KEYWORDS=${META_KEYWORDS}" \
				"-DHAVE_BASE_STYLE=${HAVE_BASE_STYLE}" \
				"-DBASE_STYLE=${BASE_STYLE}" \
				"-DHAVE_THEME_STYLE=${HAVE_THEME_STYLE}" \
				"-DTHEME_STYLE=${THEME_STYLE}" \
				"-DHAVE_CONTENT_STYLE=${HAVE_CONTENT_STYLE}" \
				"-DCONTENT_STYLE=${CONTENT_STYLE}" \
				"-DTHEME_NAV=${THEME_NAV}" \
				"-DIMAGES=${SITE_PREFIX}images" \
				"-DGLYPH=${SITE_PREFIX}glyph" \
				"-DTHUMBS=${SITE_PREFIX}thumbs" \
				"-DDOWNLOADS=${SITE_PREFIX}downloads" \
				-undef \
				-P \
				-Itemplate \
				-Icontent/${CONTENT_DIR} \
				template/template.html | sed -e "s/\\\\doubleshash/\/\//g" -e "s/\`\`/\"/g" -e "s/\`/\'/g" > site/${THEME_ROOT}/${CONTENT_DIR}.html || exit -1
		done
	fi
done

echo "Installing .htaccess..."
echo "deny from all" > site/.htaccess

echo "The web site is in ${PWD}/site"

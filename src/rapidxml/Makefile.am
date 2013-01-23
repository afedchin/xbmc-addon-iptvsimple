noinst_LTLIBRARIES = librapidxml.la

librapidxml_la_SOURCES = rapidxml.hpp \
                    rapidxml_iterators.hpp \
                    rapidxml_print.hpp \
                    rapidxml_utils.hpp

$(LIB): librapidxml.la
	cp -f .libs/librapidxml.a .
	cp -f .libs/librapidxml.la $(LIB)


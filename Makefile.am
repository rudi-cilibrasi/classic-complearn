# Copyright (c) 2006 Rudi Cilibrasi, Rulers of the RHouse
# All rights reserved.     cilibrar@cilibrar.com
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     # Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     # Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     # Neither the name of the RHouse nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE RULERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE RULERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SUBDIRS= src tests po

ACLOCAL_AMFLAGS = -I m4

DISTCLEAN_TARGETS = config.log config.status autom4te.cache \
                    complearn-gomp.pc complearn-gomp.pc

distclean: clean
	$(MAKE) -C src distclean ; rm -rf $(DISTCLEAN_TARGETS)

if FOUND_DOXYGEN
autodox:
	make -C src autodox
endif

pkgconfigdir = $(libdir)/pkgconfig
if USING_GOMP
pkgconfig_DATA = complearn-gomp.pc
else
pkgconfig_DATA = complearn.pc
endif

m4datadir = $(datadir)/aclocal
m4files = complearn.m4
m4data_DATA = $(m4files)

EXTRA_DIST = config.rpath  config.rpath m4/ChangeLog  \
$(src) \
$(examples) \
$(m4files) \
$(doc)


man_MANS = \
doc/man/complearn.5        \
doc/man/anycompress.1      \
doc/man/anydecompress.1    \
doc/man/complearn-config.1 \
doc/man/ncd.1

src = \
complearn.pc.in \
complearn-gomp.pc.in \
Doxyfile.in \
doxy/real-compressor.c       \
doxy/complearn-environment.c

doc = \
doc/man/complearn.5         \
doc/man/anycompress.1       \
doc/man/anydecompress.1     \
doc/man/complearn-config.1  \
doc/man/ncd.1               \
doc/ncd-server-protocol.txt \
THANKS

examples = \
examples/10-mammals/blueWhale.txt \
examples/10-mammals/cat.txt \
examples/10-mammals/chimpanzee.txt \
examples/10-mammals/finWhale.txt \
examples/10-mammals/graySeal.txt \
examples/10-mammals/harborSeal.txt \
examples/10-mammals/horse.txt \
examples/10-mammals/human.txt \
examples/10-mammals/mouse.txt \
examples/10-mammals/rat.txt \
examples/34-mammals/aardvark \
examples/34-mammals/armadillo \
examples/34-mammals/baboon \
examples/34-mammals/blueWhale \
examples/34-mammals/cat \
examples/34-mammals/chimpanzee \
examples/34-mammals/cow \
examples/34-mammals/dog \
examples/34-mammals/donkey \
examples/34-mammals/elephant \
examples/34-mammals/fatDormouse \
examples/34-mammals/finWhale \
examples/34-mammals/fruitBat \
examples/34-mammals/gibbon \
examples/34-mammals/gorilla \
examples/34-mammals/graySeal \
examples/34-mammals/guineaPig \
examples/34-mammals/harborSeal \
examples/34-mammals/hippopotamus \
examples/34-mammals/horse \
examples/34-mammals/human \
examples/34-mammals/indianRhinoceros \
examples/34-mammals/mouse \
examples/34-mammals/opossum \
examples/34-mammals/orangutan \
examples/34-mammals/pig \
examples/34-mammals/pigmyChimpanzee \
examples/34-mammals/platypus \
examples/34-mammals/rabbit \
examples/34-mammals/rat \
examples/34-mammals/sheep \
examples/34-mammals/squirrel \
examples/34-mammals/wallaroo \
examples/34-mammals/whiteRhinoceros \
examples/README \
examples/sampleapp/AUTHORS \
examples/sampleapp/ChangeLog \
examples/sampleapp/configure.ac \
examples/sampleapp/Makefile.am \
examples/sampleapp/NEWS \
examples/sampleapp/README \
examples/sampleapp/src/Makefile.am \
examples/sampleapp/src/rtest.c \
examples/speeches/bush-911-address \
examples/speeches/edkennedy-RFK-eulogy \
examples/speeches/fdr-first-inaugural \
examples/speeches/fdr-pearl-harbor \
examples/speeches/jfk-civil-rights-address \
examples/speeches/jfk-cuban-missile-crisis \
examples/speeches/jfk-inaugural-address \
examples/speeches/lincoln-gettysburg \
examples/speeches/mlk-i-have-a-dream \
examples/speeches/mlk-mountaintop \
examples/speeches/mx-ballot-or-bullet \
examples/speeches/reagan-challenger \
examples/speeches/reagan-evil-empire \
examples/speeches/reagan-first-inaugural \
examples/speeches/rfk-mlk-assassination \
examples/speeches/troosevelt-muckrake \
examples/speeches/wilson-inaugural \
examples/speeches/wilson-war-message

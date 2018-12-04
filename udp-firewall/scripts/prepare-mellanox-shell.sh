#!/bin/bash -e
#
# Copyright (c) 2016-2017 Haggai Eran, Gabi Malka, Lior Zeno, Maroun Tork
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

tarball=$(readlink -f ${1:-newton_ku060_2_40g_v640.tar})

if [ "$#" -lt 1 ] ; then
  echo "Missing argument: tarball"
  echo "Using: $tarball as a default"
fi

if [ ! -f "$tarball" ] ; then
  echo "File not found: $tarball"
  exit 1
fi

rm -rf user
mkdir -p user
cd user

dirs=({examples/exp_hls/{vlog,xdc},mlx,project,scripts,tb/exp_vlog})
tar xvf $tarball ${dirs[@]}
cd ..
#patch -p1 -d user < ../scripts/mellanox-shell-scripts.patch

ln -snf examples/exp_hls user/sources
ln -snf ../../../sysvlog user/sources/
ln -snf ../../../../p4/build/firewall/XilinxSwitch/XilinxSwitch_vivado/XilinxSwitch/XilinxSwitch.srcs/sources_1/imports/ user/sources/ip_repo

ln -snf ../../../../xci user/examples/exp_hls/xci
cd user/examples/exp_hls/vlog
cp -sf ../../../../../verilog/* .
cp -sf ../../../../../hls/firewall/40Gbps/impl/ip/hdl/verilog/*.v .

cd -

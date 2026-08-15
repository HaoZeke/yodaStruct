{
  lib,
  stdenv,
  meson,
  ninja,
  pkg-config,
  eigen,
  blas,
  lapack,
  libhwy,
  lua5_4,
  llvmPackages,
  seams-core-src,
}:

stdenv.mkDerivation {
  pname = "dseams";
  version = "2.2.0";

  src = lib.fileset.toSource {
    root = ./..;
    fileset = lib.fileset.unions [
      ../meson.build
      ../src
      ../lua
      ../example_lua
      ../input
      ../templates
      ../lua_inputs
    ];
  };

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
    lua5_4
  ];

  buildInputs = [
    eigen
    blas
    lapack
    libhwy
    lua5_4
  ]
  ++ lib.optionals stdenv.cc.isClang [ llvmPackages.openmp ];

  mesonAutoFeatures = "disabled";

  postPatch = ''
    mkdir -p subprojects
    cp -r ${seams-core-src} subprojects/seams-core
    chmod -R u+w subprojects/seams-core
  '';

  doCheck = true;

  postInstall = ''
    mkdir -p $out/share/lua/${lua5_4.luaversion} $out/lib/lua/${lua5_4.luaversion}
    ln -s $out/share/luadseams/lua/dseams.lua $out/share/lua/${lua5_4.luaversion}/dseams.lua
    ln -s $out/share/luadseams/lua/yoda.lua $out/share/lua/${lua5_4.luaversion}/yoda.lua
    if [ -e $out/lib/dseams_core.so ]; then
      ln -s $out/lib/dseams_core.so $out/lib/lua/${lua5_4.luaversion}/dseams_core.so
    fi
    mkdir -p $out/nix-support
    cat > $out/nix-support/setup-hook <<EOF
    addToSearchPath LUA_PATH "$out/share/luadseams/lua/?.lua"
    addToSearchPath LUA_CPATH "$out/lib/?.so"
    EOF
  '';

  meta = {
    description = "d-SEAMS Lua and Fennel library (require(\"dseams\"))";
    homepage = "https://github.com/d-SEAMS/yodaStruct";
    license = lib.licenses.mit;
    platforms = lib.platforms.linux ++ lib.platforms.darwin;
  };
}

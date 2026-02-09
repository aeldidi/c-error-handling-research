{ pkgs }:

let
  inherit (pkgs) fetchurl runCommand symlinkJoin;

  linux =
    runCommand "linux-6.18.9"
      {
        src = fetchurl {
          url = "https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.18.9.tar.xz";
          sha256 = "sha256-AwEV/4+0y1NthEncQOvD4xToa6GzFqauIQkaEcyTBXg=";
        };
      }
      ''
        mkdir $out
        tar -xf $src --strip-components=1 -C $out
      '';

  postgresql =
    runCommand "postgresql-18.1"
      {
        src = fetchurl {
          url = "https://ftp.postgresql.org/pub/source/v18.1/postgresql-18.1.tar.gz";
          sha256 = "sha256-sPGMLWlz0qoCPPx3/tp4fXu+nDGjl30PBKwpiF+5jsQ=";
        };
      }
      ''
        mkdir $out
        tar -xf $src --strip-components=1 -C $out
      '';

  openbsd =
    runCommand "openbsd-7.8"
      {
        srcs = [
          (fetchurl {
            url = "https://cdn.openbsd.org/pub/OpenBSD/7.8/sys.tar.gz";
            sha256 = "sha256-4kkEyq7kDod0gWsg8wmDYm9KVDTgS0doqe9FHcS8Oes=";
          })
          (fetchurl {
            url = "https://cdn.openbsd.org/pub/OpenBSD/7.8/src.tar.gz";
            sha256 = "sha256-HyDMYfO1vJvmm+Ex+ciXY1ymw7Tro6aek4po50E59Io=";
          })
        ];
      }
      ''
        mkdir $out
        for s in $srcs; do
          tar -xf "$s" -C $out
        done
      '';

  freebsd =
    runCommand "freebsd-15.0"
      {
        src = fetchurl {
          url = "https://download.freebsd.org/releases/amd64/15.0-RELEASE/src.txz";
          sha256 = "sha256-g8PoFXttevyuVxZ/2nVpO/Hl9YHKFJpuyy05i3G9+rA=";
        };
      }
      ''
        mkdir $out
        tar -xf $src -C $out
      '';

  netbsd =
    runCommand "netbsd-10.1"
      {
        srcs = [
          (fetchurl {
            url = "https://cdn.netbsd.org/pub/NetBSD/NetBSD-10.1/source/sets/syssrc.tgz";
            sha256 = "sha256-dqYA5wPS6WR1MyPiZNPsB9DGy+E0ZI/I8PE+2fqqG+Q=";
          })
          (fetchurl {
            url = "https://cdn.netbsd.org/pub/NetBSD/NetBSD-10.1/source/sets/src.tgz";
            sha256 = "sha256-T5y+h89+yQJDRc1DOQjTVbS5HiKx2sYrN1F5NS5r45k=";
          })
        ];
      }
      ''
        mkdir $out
        for s in $srcs; do
          tar -xf "$s" -C $out
        done
      '';

  mysql =
    runCommand "mysql-8.4.0"
      {
        src = fetchurl {
          url = "https://downloads.mysql.com/archives/get/p/23/file/mysql-8.4.0.tar.gz";
          sha256 = "sha256-R6VDP83WOduDa5nhtUWcK4E8va0j/ytd1K0n95K6kY4=";
        };
      }
      ''
        mkdir $out
        tar -xf $src --strip-components=1 -C $out
      '';

in

symlinkJoin {
  name = "datasets";
  paths = [
    linux
    postgresql
    openbsd
    freebsd
    netbsd
    mysql
  ];
}

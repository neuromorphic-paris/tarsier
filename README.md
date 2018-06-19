![tarsier](banner.png "The Tarsier banner")

Tarsier is a collection of tools to build event-based algorithms. It is an header-only library.

# install

Within a Git repository, run the commands:

```sh
mkdir -p third_party
cd third_party
git submodule add https://github.com/neuromorphic-paris/tarsier.git
git submodule update --init --recursive
```

# user guides and documentation

User guides and code documentation are held in the [wiki](https://github.com/neuromorphic-paris/tarsier/wiki).

# contribute

## development dependencies

### Debian / Ubuntu

Open a terminal and run:
```sh
sudo apt install premake4 # cross-platform build configuration
sudo apt install clang-format # formatting tool
```

### macOS

Open a terminal and run:
```sh
brew install premake # cross-platform build configuration
brew install clang-format # formatting tool
```
If the command is not found, you need to install Homebrew first with the command:
```sh
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

## test

To test the library, run from the *tarsier* directory:
```sh
premake4 gmake
cd build
make
cd release
./tarsier
```

After changing the code, format the source files by running from the *tarsier* directory:
```sh
for file in source/*.hpp; do clang-format -i $file; done;
for file in test/*.cpp; do clang-format -i $file; done;
```

# license

See the [LICENSE](LICENSE.txt) file for license rights and limitations (GNU GPLv3).

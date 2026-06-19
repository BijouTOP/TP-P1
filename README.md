## Repositório

```bash
git clone https://github.com/BijouTOP/TP-P1.git
cd TP-P1
```

## Dependências

O projeto utiliza:

- Raylib 6.0
- RayGUI
- libcurl
- CMake

---

## Linux

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install build-essential cmake git libcurl4-openssl-dev
```

### Compilação

```bash
mkdir build
cd build

cmake ..
cmake --build .
```

### Execução

```bash
./TP-P1
```

---

## Windows

### Requisitos

- Visual Studio 2022 (Desktop Development with C++)
- CMake
- Git

### Dependência libcurl

Caso o CMake não encontre automaticamente o CURL:

- Instalar através do vcpkg

```powershell
vcpkg install curl:x64-windows
```

ou

- Instalar uma distribuição que inclua libcurl (MSYS2/MinGW)

### Compilação

```powershell
mkdir build
cd build

cmake ..
cmake --build . --config Release
```

### Execução

```powershell
.\Release\TP-P1.exe
```

## Nota

Caso o Raylib não esteja instalado no sistema, o CMake descarrega e compila automaticamente a versão 6.0 através de FetchContent.

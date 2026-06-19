# TP-P1 — Mini NOC

Aplicação desktop para gestão de uma pequena rede / NOC (Network Operations Center): inventário de equipamentos, monitorização de sensores, registo de incidentes técnicos e histórico de configurações, com interface gráfica em raylib/raygui.

## Repositório

```bash
git clone https://github.com/BijouTOP/TP-P1.git
cd TP-P1
```

---

## Bibliotecas e ferramentas utilizadas

| Ferramenta / Biblioteca | Função no projeto |
|---|---|
| **C11** | Linguagem do projeto |
| **[raylib](https://www.raylib.com/) 6.0** | Janela, loop gráfico, input, áudio (música de fundo) |
| **[raygui](https://github.com/raysan5/raygui)** | Widgets de interface (botões, dropdowns, scroll panels, text boxes) sobre o raylib |
| **libcurl** | Pedidos HTTP para importar leituras de sensores a partir de uma API externa |
| **CMake (≥ 3.11)** | Sistema de build, com `FetchContent` para descarregar e compilar a raylib automaticamente caso não esteja instalada no sistema |
| **GCC / MinGW-w64 / MSVC** | Compiladores suportados (Linux, Windows) |

---

## Compilação

> Em todos os SOs, se a raylib 6.0 não estiver instalada no sistema, o CMake descarrega-a e compila-a automaticamente via `FetchContent` — não é necessário instalar a raylib manualmente.

### Linux (testado)

Os passos abaixo são praticamente idênticos em qualquer distribuição — muda apenas o gestor de pacotes e o nome dos pacotes de dependências de build.

**Debian / Ubuntu**
```bash
sudo apt update
sudo apt install build-essential cmake git libcurl4-openssl-dev
```

**Fedora**
```bash
sudo dnf install gcc make cmake git libcurl-devel
```

**Arch Linux**
```bash
sudo pacman -S base-devel cmake git curl
```

**openSUSE**
```bash
sudo zypper install gcc make cmake git libcurl-devel
```

Compilação (igual em todas as distribuições). Primeiro navega até à pasta do projeto (caso ainda não estejas lá, por exemplo após o `git clone`):
```bash
cd TP-P1
```

Depois:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

O executável é colocado na raiz do projeto (não dentro de `build/`). Execução:
```bash
cd ..
./TP-P1
```

---

### Windows — MSYS2/MinGW (testado)

1. Instalar o [MSYS2](https://www.msys2.org/) (instalador oficial, opções por defeito).
2. Abrir o terminal **"MSYS2 MinGW x64"** a partir do menu Iniciar (tem de ser este especificamente, não o "MSYS2 MSYS" nem o "MSYS2 UCRT64").
3. Atualizar o sistema e instalar as dependências:
   ```bash
   pacman -Syu
   # Se o terminal fechar, reabrir o "MSYS2 MinGW x64" e repetir:
   pacman -Syu

   pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-curl git
   ```
4. Navegar até à pasta do projeto (o MSYS2 mapeia `C:\` para `/c/`). Por exemplo, se o projeto estiver em `C:\Users\NomeDeUtilizador\Downloads\TP-P1`:
   ```bash
   cd /c/Users/NomeDeUtilizador/Downloads/TP-P1
   ```
5. Compilar:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build .
   ```
6. O executável (`TP-P1.exe`) é colocado na raiz do projeto, junto às pastas `media/` e `data/`. Executar:
   ```bash
   cd ..
   ./TP-P1.exe
   ```

---

### Windows — Visual Studio + vcpkg (não testado)

1. Instalar [CMake](https://cmake.org/download/), [Git](https://git-scm.com/) e o [Visual Studio](https://visualstudio.microsoft.com/) com a workload "Desenvolvimento para Desktop com C++".
2. Instalar o libcurl via [vcpkg](https://github.com/microsoft/vcpkg):
   ```powershell
   vcpkg install curl:x64-windows
   ```
3. Compilar, apontando o CMake para o toolchain do vcpkg:
   ```powershell
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[caminho-para-vcpkg]/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```
4. Executar (o executável é colocado na raiz do projeto):
   ```powershell
   cd ..
   .\TP-P1.exe
   ```

---

## Manual de utilização

Ao iniciar, a aplicação cria automaticamente uma pasta `data/` na raiz do projeto (junto ao executável) onde guarda toda a informação em ficheiros binários/texto, e carrega os dados existentes dessa pasta, se já existirem de uma execução anterior.

### Menu principal
Ecrã inicial com acesso aos quatro módulos da aplicação. Ícones no canto: ecrã inteiro (topo direito), música ligar/desligar e sair da aplicação (fundo direito), e voltar ao menu principal (fundo esquerdo, disponível dentro de cada módulo).

### Inventário de Equipamentos
- Listagem de equipamentos de rede (routers, switches, access points, servidores, NAS, impressoras, câmaras, sensores, UPS).
- Adicionar / remover equipamentos, com código interno, marca, modelo, IP, MAC, localização e estado (operacional, manutenção, falha, desativado).
- **Teste de conectividade (ping)** a um equipamento individual ou a todos de uma vez, usando o `ping` do sistema operativo.
- Exportar/importar inventário (ficheiro binário).

### Monitorização de Sensores
- Importa leituras de sensores automaticamente de uma **API REST** ao iniciar a aplicação; se a API estiver indisponível, carrega um ficheiro de backup local (`sensores_rack.txt`).
- Pesquisa e filtro por estado do sensor (normal, aviso, crítico, falha de rede).
- Exportação/importação manual de leituras.

### Incidentes Técnicos
- Registo de incidentes associados a equipamentos do inventário (criação automática de incidente ao assinalar uma falha num equipamento).
- Prioridade (baixa/média/alta) e estado (pendente, em curso, concluído).
- Histórico com data de criação e de conclusão, e técnico responsável.

### Registos de Configuração
- Histórico de alterações de configuração a equipamentos (valor anterior → novo valor, técnico, data/hora).
- **Undo/Redo** de alterações de configuração.

### Relatórios
- Geração automática de relatório de estado da rede (equipamentos operacionais vs. em falha, incidentes pendentes).
- Geração de relatório mensal de incidentes.
- Relatórios guardados como ficheiros de texto em `data/`.

Ao fechar a aplicação (botão de sair), todos os dados são automaticamente gravados em `data/`.

---

## Descrição das atividades extra implementadas

Além dos requisitos mínimos do trabalho, foram implementadas as seguintes funcionalidades adicionais:

- **Integração com API externa via libcurl** — importação automática de leituras de sensores a partir de um endpoint HTTP remoto, com *fallback* transparente para um ficheiro local de backup caso a API esteja indisponível.
- **Teste de conectividade real (ping)** a equipamentos do inventário, individual ou em lote, invocando o comando `ping` do sistema operativo e interpretando o resultado (compatível com Linux e Windows).
- **Undo/Redo** no histórico de registos de configuração.
- **Interface gráfica completa** (em vez de uma interface em linha de comandos), com múltiplos menus, scroll, pesquisa/filtros, ícones interativos e suporte para ecrã inteiro e redimensionamento de janela.
- <span style="font-size: 2em;">***Música de fundo*** com opção de mutar/desmutar.</span>
- **Build multiplataforma** (Linux e Windows) com o mesmo `CMakeLists.txt`, incluindo descarregamento e compilação automática da raylib via `FetchContent` quando não está instalada no sistema.

---

## Licença

Este projeto está licenciado sob a [GNU General Public License v3.0](LICENSE).

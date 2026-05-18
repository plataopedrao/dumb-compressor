# Dumb Compressor — Project Knowledge

## Visão Geral
Plugin de áudio VST3 inspirado no compressor UREI 1176 (CLA-76).  
Desenvolvido pela **Peposoft GmbH** por Pedro (plataopedrao).

## Stack
| Item | Detalhe |
|------|---------|
| Linguagem | C++17 |
| Framework | JUCE 7.0.12 (via CMake FetchContent) |
| Build system | CMake + Ninja |
| Compilador | MSVC 19.44 (VS Build Tools 2022) |
| Formatos | VST3 + Standalone (.exe) |

## Estrutura do Projeto
```
C:\projetos\dumb compressor\
├── CMakeLists.txt          — configuração do build (JUCE, formatos, parâmetros)
├── build.bat               — script de build + instalação automática do VST3
├── PROJECT.md              — este arquivo
└── Source\
    ├── PluginProcessor.h   — declaração do AudioProcessor + APVTS
    ├── PluginProcessor.cpp — DSP: compressor feed-forward estilo 1176
    ├── PluginEditor.h      — declaração da UI (dois knobs)
    └── PluginEditor.cpp    — UI: knob OUTPUT (esquerda) + knob RATIO (direita)
```

## Parâmetros
| ID | Nome | Tipo | Valores |
|----|------|------|---------|
| `ratioIndex` | Ratio | Float discreto 0–4 | Off / 4:1 / 8:1 / 12:1 / 20:1 |
| `outputLevel` | Output | Float discreto 0–4 | 0% / 25% / 50% / 75% / 100% |

## DSP — Compressor Feed-Forward (estilo 1176)
- **Topologia**: feed-forward — detector age sobre o sinal de entrada, antes da redução de ganho
- **Threshold**: -18 dBFS (fixo)
- **Attack**: 1ms (rápido, emula FET do 1176)
- **Release**: 100ms (fixo)
- **Knee**: hard
- **Envelope**: por canal L/R independente
- **Fórmula de ganho**:
  ```
  overDb = level_dB - threshold_dB
  gainDb = overDb * (1/ratio - 1)   // sempre negativo quando ratio > 1
  gain   = dBToLinear(gainDb)
  ```
- **Ratio = Off (0)**: bypass da compressão; output level ainda aplicado

## Regras de Build
1. **Compilar sempre em**: `C:\projetos\dumb compressor\`
2. Executar: `build.bat` (configura MSVC x64, roda cmake + ninja, Release)
3. **Após build**, copiar automaticamente o VST3 para:  
   `C:\Program Files\Common Files\VST3\Peposoft\Dumb Compressor.vst3`  
   *(requer UAC — uma janela de elevação aparece)*
4. Commitar e fazer push para o GitHub após toda alteração

## Repositório GitHub
- URL: https://github.com/plataopedrao/dumb-compressor
- Branch principal: `master`
- Email dos commits: `pedro@woliveira.net`
- Usuário: `plataopedrao`

## Instalação do VST3
| Local | Caminho |
|-------|---------|
| Instalado (Peposoft) | `C:\Program Files\Common Files\VST3\Peposoft\Dumb Compressor.vst3` |
| Build output | `C:\projetos\dumb compressor\build\DumbCompressor_artefacts\Release\VST3\` |
| Standalone (.exe) | `C:\projetos\dumb compressor\build\DumbCompressor_artefacts\Release\Standalone\Dumb Compressor.exe` |

## Problema Conhecido — MSVC Libs Zeroed
Durante uma atualização do VS Build Tools (winget), os arquivos:
- `libcpmt.lib` / `libcpmtd.lib`
- `libvcruntime.lib` / `libvcruntimed.lib`

foram zerados (arquivo existe mas contém só zeros).  
**Solução aplicada**: extrair os arquivos corretos do cache do VS em  
`C:\ProgramData\Microsoft\VisualStudio\Packages\Microsoft.VC.14.44.17.14.CRT.x64.Desktop.base,version=14.44.35226\payload.vsix`  
e copiá-los com elevação para  
`C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.44.35207\lib\x64\`

## Patch do juceaide (JUCE 7)
O JUCE 7 compila o `juceaide` em modo Debug durante o configure.  
Se as debug libs estiverem quebradas, aplicar patch em:  
`build\_deps\juce-src\extras\Build\juceaide\CMakeLists.txt`  
Trocar `Debug` → `Release` nas linhas de `CMAKE_BUILD_TYPE` e `--config`, e  
`IMPORTED_LOCATION_DEBUG` → `IMPORTED_LOCATION_RELEASE`.

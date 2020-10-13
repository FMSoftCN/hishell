# HiShell

HiShell is the shell for the HybridOS.

## Table of Contents

- [How to Build ?](#how-to-build)


## How to Build

It compiles as part of [HybridOS Device Side]. 

- Build the Application

  ```bash
  # cd hishell
  # cmake .
  # maske
  ```

  The output directory is "hishell\bin". All files the OS depends on,  will be in this folder, executable files, configuration files,  and some bash files are inclusive.

- Run
  In dirctory "hishell/bin", type:

  ```bash
  # cd hishell/bin
  # source env.sh
  # ./mginit
  ```

---
[HybridOS Device Side]: /hybridos/hybridos/tree/master/device-side

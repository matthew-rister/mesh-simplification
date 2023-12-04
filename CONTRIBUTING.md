# CONTRIBUTING

Welcome 👋 This document outlines standards and guidelines for contributing to this project.

## Issue Templates

This project makes use of GitHub issue templates. If you encounter a problem, please use the Bug Report issue template. Similarly, feature requests should be made through the Work Item template.

## Getting Started

In addition to the dependencies listed in the [Requirements](README.md#Requirements) section of the README, contributing to this project will require:

- [python3](https://www.python.org/downloads/)
  - [pre-commit](https://pre-commit.com)
  - [cpplint](https://github.com/cpplint/cpplint)
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)

## Pre-commit

This project uses [pre-commit](https://pre-commit.com) to integrate essential [git hooks](https://git-scm.com/book/en/v2/Customizing-Git-Git-Hooks) that will validate project requirements prior to checking in. After following the installation instructions, run `pre-commit install` from the root directory which enable pre-commit hooks defined in [.pre-commit-config.yaml](.pre-commit-config.yaml).

## Code Style

This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with the following exceptions:

1. 120-character limit per line.
2. Exceptions are allowed.
3. Forward declarations are allowed.
4. Objects with static storage duration are allowed.
5. APIs are documented with [Doxygen](https://www.doxygen.nl/).

Code style is enforced with `clang-format` and `cpplint` which are run prior to check-in using pre-commit hooks.

## Commit Message Template

To promote descriptive commits, this project uses a commit message template which should be configured by running:

```bash
git config commit.template .gitmessage
```

### Commits Message Format

A well-crafted commit message should consist of a **header**, **body**, and optional **footer**.

#### Header

The header should be a one-line summary of the form:

```text
<type>: <summary>
 │       │
 │       └─⫸ summary: present tense without punctuation
 │
 └─⫸ type: build|docs|feat|fix|refactor|style|test
```

Commit changes can be categorized into the following types:

| Type       | Description                                          |
| -------    | ---------------------------------------------------- |
| `build`    | Changes a CMake script or build process              |
| `docs`     | Updates project or source code documentation         |
| `feat`     | Adds a new feature for a user                        |
| `fix`      | Fixes a user facing defect                           |
| `refactor` | Alters code structure without changing functionality |
| `style`    | Applies formatting or changes code style conventions |
| `test`     | Relates to tests and testing infrastructure          |

#### Body

The commit body should describe the motivation behind a code change and explain how the change addresses the problem. It can also include relevant notes such as how the change was tested or links to additional technical documentation.

#### Footer

The footer is optional and can include references to related GitHub issues affected by the commit.

#### Example

```text
build: compile vcpkg libraries with ASan



When compiling address sanitizer on Windows, external libraries managed

by vcpkg will fail to compile with the following error: "error LNK2038:

mismatch detected for 'annotate_string': value '0' doesn't match value

'1'". This is caused by enhanced ASan instrumentation for the standard

library which creates conflicting symbols when also used by an external

library due to ODR.



Although this functionality can be disabled with macro definitions

(_DISABLE_VECTOR_ANNOTATION and _DISABLE_STRING_ANNOTATION), a better

solution is to build external libraries managed by vcpkg with ASan

enabled. To achieve this, custom vcpkg triplets are created for Windows

x64 and x86 which set the VCPKG_CXX_FLAGS_DEBUG and VCPKG_C_FLAGS_DEBUG

to use /fsanitize=address. These custom triplets can then be enabled by

first configuring vcpkg to know about them in vcpkg-configuration.json

and then setting CMake cache variables to use them in CMakePresets.json.



Closes #13
```

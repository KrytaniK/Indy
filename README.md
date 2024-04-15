<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->

<a name="readme-top"></a>

<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/krytanik/indy">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">[WIP] Indy Game Engine</h3>

  <p align="center">
    Real-time, procedural game development.
    <br />
    <a href="https://github.com/krytanik/indy"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="#">View Demo</a>
    <!-- ·
    <a href="https://github.com/krytanik/indy/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    ·
    <a href="https://github.com/krytanik/indy/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a> -->
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li>
            <a href="#motivation">Motivation</a>
        </li>
        <li>
            <a href="#built-with">Built With</a>
            <ul>
                <li><a href="#tools-and-frameworks">Tools and Frameworks</a></li>
                <li><a href="#languages">Languages</a></li>
            </ul>
        </li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## About The Project

<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

Currently, Indy is in early development with most of its features still on the planning board. You can find a detailed list of all planned features [below](#roadmap).

As of now, Indy is only supported on Windows machines.

### Motivation

Being fascinated with games for many years initially led me to pursue game development in the traditional sense. However, after several prototypes and small projects, I decided that I wanted more refined control over an engine's behavior. I didn't want all of the fluff that comes with modern editors.

In addition to my desire to simplify the process, I want the ability for core systems to be opt-in. This means that you shouldn't be tied down to what's immediately available to you. Developers should have the choice to use existing systems or build their own. No questions asked.

Aside from my frustrations, my primary goal with Indy is to learn, and to ultimately build a full game. This is one thing on my bucket list of stuff that I want to do. I'm fascinated with how game engines work under the hood, and I feel that first understanding how these systems work together will help me create a better game.

### Built With

#### Tools and Frameworks

- [GLFW][glfw-url]
- [Vulkan][vulkan-url]
- [Premake][premake-url]

#### Languages

- C/C++
- Lua

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- GETTING STARTED -->

## Getting Started

Currently, the only way to run Indy is to build the engine from source. Once the infrastructure is in place, I plan to release a packaged version.

### Prerequisites

To get the best experience with Indy, it is recommended to use the latest version of [Visual Studio][vs-url], utilizing Microsoft's MSVC compiler, as this will provide the most stable build.

Other compilers either do not yet support C++ modules, or only have partial implementations. [Support for C++ header files](#roadmap) is planned to account for this.

### Installation

Installing and running Indy has been made as easy as possible. So long as you have the latest version of Visual Studio, there are only two steps.

#### Windows

1. Clone the repo into your directory of choice:
   ```sh
   git clone https://github.com/krytanik/indy.git
   ```
2. To generate the Visual Studio solution with all required dependencies, you can run the premake scripts by double clicking GenerateProjects.bat, or running a simple command in your terminal:
   ```sh
   ./indy/GenerateProjects.bat
   ```

#### Mac & Linux

Mac and Linux are not yet [supported](#roadmap).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

## Usage

Since a game engine can be quite verbose, I'm building a [[WIP] website][docs-url] to provide detailed documentation for Indy's core systems.

If you're curious about the website's progress, you can view the public repo [here][docs-site-url].

<!-- _For more examples, please refer to the [Documentation](https://example.com)_ -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- ROADMAP -->

## Roadmap

Indy is still very early in its development. Below is where the core library currently stands:

- [x] Events
- [x] Input
- [ ] Render Pipeline < In Progress
- [ ] Time
- [ ] User Interface
- [ ] Platform Support
  - [ ] Mac
  - [ ] Linux

These are only some of the planned features that haven't yet made their way to the planning board:

- [ ] Minimalistic Editor
- [ ] Entity-Component System
- [ ] Custom Shaders/Materials

_To stay up to date on latest changes, check out Indy's [Documentation][docs-url]_

See the [open issues](https://github.com/krytanik/indy/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/krytanik/indy.svg?style=for-the-badge
[contributors-url]: https://github.com/krytanik/indy/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/krytanik/indy.svg?style=for-the-badge
[forks-url]: https://github.com/krytanik/indy/network/members
[stars-shield]: https://img.shields.io/github/stars/krytanik/indy.svg?style=for-the-badge
[stars-url]: https://github.com/krytanik/indy/stargazers
[issues-shield]: https://img.shields.io/github/issues/krytanik/indy.svg?style=for-the-badge
[issues-url]: https://github.com/krytanik/indy/issues
[license-shield]: https://img.shields.io/github/license/krytanik/indy.svg?style=for-the-badge
[license-url]: https://github.com/krytanik/indy/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/kyle-wick
[vs-url]: https://visualstudio.microsoft.com/vs/
[premake-url]: https://premake.github.io
[vulkan-url]: https://www.vulkan.org
[glfw-url]: https://www.glfw.org
[docs-url]: https://indy-docs-f1y4-j4mj2i81m-krytaniks-projects.vercel.app
[docs-site-url]: https://github.com/KrytaniK/Indy-Docs

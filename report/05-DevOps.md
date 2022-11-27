\newpage
# DevOps
In order to automate the build process of documentation, executable, and report a *Continuous Integration* worflow was designed.

## Nix
The first step of the worlflow is composed of a package manager called [*Nix*](https://nixos.org/). It is designed to create environments ensuring reproducibility across different machines.

The behavior of nix commands is described inside the `flake.nix` file, it also contains all the dependencies required for the execution of each possible scenario. To enforce the reproducibility in the future all required packages are fixed inside the `flake.lock` file.

The build entrypoints offered by nix are:

* `nix build .#report`: builds this report from Markdown files using pandoc and a latex template;
* `nix build .#particle-swarm-optimization`: builds the final binary;
* `nix build .`: same as `nix build .#particle-swarm-optimization`.

There exists also a development entrypoint that can be called using the command `nix shell`, it provides a new shelll containing all dependencies required for build, testing, and report.

## Docker
The reproducibility during execution of the final binary is providede by *containers*. A container is sandobox that contains all runtime dependencies required by the executable placed within it.

A manager for the life-cycle of containers is required, we opted for *Docker*, a state-of-the-art software used in many environments. It builds an image from the descriptor file `Dockerfile` placed in the root of the project, then from the image it is possible to create several containers that are indipendent of each other.

The main problem encauntered was the root permission required by docker for the build operation. This prevented us to use it inside the cluster. To overcome this requirement we had to setup other technologies described in the following sections.

## GitHub actions
A *GitHub action* is a list of commands executed when a specific event is triggered within a repository. It is possible to define multiple actions (also on different files) inside the `.github/workflows` directory.

We defined two worflows, one for the container creation and the other for the documentation compilation. They are triggered on each commit.

### Container creation
The container creation worflow uses the `Dockerfile` to build up a docker image (as explained in [Section 6.2](#Docker)) that is then pushed to a *container registy* called *DockerHub*. The process also includes an automation that executes unit tests, if only one them is marked as failed then the whole process is interrupted to avoid runtime errors in production.

![Container creation worflow](./images/container_creation.png){ width=450px }

### Documentation compilation
The documentation is compiled in two formats:

* **html**: it includes all the content of this report and in addition it also provides code documentation extracted with *Doxygen* following rules defined in the `Doxyfile` file placed in the root of the project;
* **pdf**: this report.

Both formats are generated using a github action and the html website is hosted on *GitHub Pages* at [this link](https://fedeizzo.github.io/ParticleSwarmOptimization-OpenMPI/).

## Udocker
The final step of the workflow is represented by [*Udocker*](https://indigo-dc.github.io/udocker/), it is a container manager like Docker but it does not require root permission for the execution phase.

During the usage inside the cluster we encountered two main problems:

* the build phase;
* the OpenMPI communication between independent sandboxes.

### Build phase
Unfortunately udocker does not offer primitives for containers building, it only provides one intake operation that is a pull from a container registry. For this reason we created the worflow described in [Section 6.3.1](#Container creation).

The pulled image created by GitHub and hosted on DockerHub can be used to create a container within the cluster. The sandbox is executed using a special environment thanks to *Fakechroot*.

![Container pull worflow](./images/container_pull.png){ width=450px }

### OpenMPI communication
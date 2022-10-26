# Particle Swarm Optimization OpenMPI

## OpeMPI
OpeMPI library is used to convey information across processes running on different nodes of a cluster. The basic information unit is composed as a broadcast message shared over the whole network, in this way all particles of Particle Swarm Optimization (PSO) are able to know all information associated to other members of the swarm.

## Process
A process can have the task of computing the algorithm for one or more particles, it is divided in several threads:

* one waiting for broadcast messages from other particles;
* one writing new broadcast messages for the particle/particles assigned to the process;
* one or more threads associated to the computation of position, fitness, velocity, etc.


TODO: schema for the communication

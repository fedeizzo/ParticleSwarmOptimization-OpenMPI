# Particle Swarm Optimization OpenMPI

## OpeMPI
OpeMPI library is used to convey information across processes running on different nodes of a cluster. The basic information unit is composed as a broadcast message shared over the whole network, in this way all particles of Particle Swarm Optimization (PSO) are able to know all information associated to other members of the swarm.

The process that produces the message send the message using a broadcast function because all particles must know the positions of other individuals of the population at every step. In the following picture it is presented a simple schema of the communication.

![Communication schema](./images/communication_schema.png)

## Process
A process can have the task of computing the algorithm for one or more particles, it is divided in several threads:

* one waiting for broadcast messages from other particles;
* one writing new broadcast messages for the particle/particles assigned to the process;
* one or more threads associated to the computation of position, fitness, velocity, etc.

Moreover there are two buffers: one for input and another for output.

![Execution schema](./images/communication_schema.png)

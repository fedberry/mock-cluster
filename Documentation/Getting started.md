# Getting started

**Mock-Cluster** is automation tool that help Fedberry team to automize RPM build process. 

# Pre zero

To activate your Zero to Pre Zero process follow next procedure:

- install repo rpm
```
rpm -ivh http://download.fedberry.org/pre_zero/RPMS/pignus-fedberry-repo-23-0.1.fc23.noarch.rpm
```
- install mock-client and wait until mock environment get initialized
```
dnf install mock-client -y
```

- now your agent is registered. Ask admin to activate it on agents page.

All done. Your agent is up and running and run tasks for Pre Zero program.

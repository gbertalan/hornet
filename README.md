# Project

## Project Design

This project follows the **MVC (Model-View-Control)** pattern with an additional **Service layer**.

### Layers
Main creates:
- **ModelAccess** (which implicitly creats the *Models*),
- **Services**,
- **View** (which creates its internal *Components*) and
- **Control**.
  
Main also connects View's *signals* to Control's *slots*.

![Design overview](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/layers.png)



- **Model**  
  Data storage only.

- **Service**  
  Business logic only.

- **View**  
  Qt GUI.

- **Controller**  
  Coordination only.


### Inter-Layer Communication

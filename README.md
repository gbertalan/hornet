# Project

## Project Design

This project follows the **MVC (Model-View-Control)** pattern with an additional **Service layer**.

---
### Layers
`Main` instantiates the core class of each layer::
- `ModelAccess` (which implicitly creats the `Model` classes),
- all `Service` classes,
- `View` (which creates its internal `Qt` components) and
- `Control`.
  
Main also connects View's *signals* to Control's *slots*.

![Design overview](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/layers.png)


#### Layer Responsibilities
- **Model**  
  Data storage only.

- **Service**  
  Business logic only.

- **View**  
  Qt GUI.

- **Controller**  
  Coordination only.

---
### Inter-Layer Communication

# Project

## Project Design

This project follows the **MVC (Model-View-Control)** pattern with an additional **Service layer**.

---
### Layers
`Main` instantiates the core class of each layer::
- **Model** layer: `ModelAccess` class (which implicitly creats the `Model` classes),
- **Service** layer: all `Service` classes,
- **View** layer: the `View` (which creates its internal `Qt` components) and
- **Control** layer: the `Control` class.
  
Main also connects View's *signals* to Control's *slots*.

![Design overview](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/layers.png)


#### Layer Responsibilities
- **Model** layer<br>
  Data storage only.

- **Service**  layer<br>
  Business logic only.

- **View**  layer<br>
  Qt GUI.

- **Control**  layer<br>
  Coordination only.

---
### Inter-Layer Communication

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
  
Main connects View's *signals* to Control's *slots*.

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
**Path #1:** View -> Control

A UI component emits a signal to its parent. The signal propagates through the component hierarchy upward to `Main`, which then delegates it to `Control`.

**Path #2:** Control -> Model

`Service` has a public API. `Control` calls a function of `Service`, `Service` has read/write access to `Model`.

**Path #3:** Model -> Control

`Control` has direct read-only access to `Model`.

**Path #4:** Control -> View

`View` has a public API. `Control` calls a function of `View`. The call propagates down the UI component hierarchy until it reaches its destination.

#### DTOs

When data is forwarded between layers, it is always contained in a **DTO**. 

The *sender* **creates** and **writes** the DTO, the *receiver* only **reads** it.

DTOs contain **values**, not pointers or references.

Consequence: functions that are used for inter-layer communication have either **no parameters**, or **one parameter** that is a **DTO**.

## Debug Print

The `Ctrl + D` shortcut prints the full contents of Model."

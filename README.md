# Project

## Project Design

This project follows the **MVC (Model-View-Control)** pattern with an additional **Service layer**.

---
### Layers
`Main` instantiates the core class of each layer::
- **Model** layer: `ModelAccess` class (which implicitly creats the `Model` classes),
- **Service** layer: all `Service` classes,
- **View** layer: the `View` (which creates its internal `Qt` components) and
- **Control** layer: the `Control` class (which internally instantiates its family of specialized control classes).
  
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

### Model Layer

The **ModelAccess** class gives access to _Models_.

There are two interfaces in this layer, **IModelAccessRead** and **IModelAccessReadWrite**. What function we place into which interface controls what can be read/written from a given model.

If the calling class has the interface IModelAccessRead included, it has reading access only.
Otherwise the interface IModelAccessReadWrite is needed for both reading and writing rights.
 
In this design **Control** has reading right, **Services** have reading and writing rights.

![Model Layer Diagram](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/model_layer.png)

#### Reading/Writing Models

---

### Service Layer

![Service Layer Diagram](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/service_layer.png)

---

### View Layer

![View Layer Diagram](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/view_layer.png)

A high level overview of the **View Layer** components.

---

### Control Layer


---

### Initialization Flow

Main creates:
- QApplication
  - Manages the GUI application's control flow and main settings
- ModelAccess
  - ModelAccess inherits from IModelAccessReadWrite,
    - IModelAccessReadWrite inherits from IModelAccessRead
  - ModelAccess internally creates:
    - All Models
- All Services
- The View
  - View creates its internal components
- The Control
  
Main connects:
- View's signals to Control's slots
  
Main calls:
- control.init()
- view.show()

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

_Functions that are used for inter-layer communication have either **no parameters**, or **one parameter** that is a **DTO**._

---
### Specific Layer Communication Patterns

#### Complex path 1: View -> Model

Signals are declared but not defined in Qt.

1. Declare a signal in the widget as a function with one DTO parameter.
2. Emit this signal from the widget when the event occurs.
3. In each ancestor widget up to **View**: declare the same signal with the same function signature, and in the ancestor's constructor connect the child's signal to the ancestor's signal.
4. In **View**: declare the same signal.
5. In `main.cpp`: connect **View**'s signal to **Control**'s slot.
6. In **Control**: the slot calls the specific sub-control's handler function (e.g. `GridControl::handleGridZoomChange`).
7. The sub-control calls the specific **Service**'s function.
8. The **Service** stores the data in **Model** via `IModelAccessReadWrite` (read+write access, used by **Service** layer). Add a setter to the relevant **Model** class, and call it via e.g. `m_modelAccess.getGridModel().setXxx()`.

![View to Model](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/view_to_model.png)

#### Complex path 2: Model -> View

1. In the sub-control (e.g. `GridControl`), after the **Service** has updated the **Model**, read the updated state back from **Model** via `IModelAccessRead` (read-only access, used by **Control** layer).
2. Package the data into a view-facing DTO (e.g. `GridViewStateDTO`).
3. Call the **View** API function directly via the `m_view` reference (e.g. `m_view.updateGridViewState(dto)`).
4. In **View**: the API function cascades the DTO down the widget hierarchy until it reaches the target widget.
5. The target widget applies the data and calls `update()` to trigger a repaint.

![Model to View](https://github.com/gbertalan/hornet/blob/main/hornet/design_images/view_to_model.png)
---

## Debug Print

The `Ctrl + D` shortcut prints the full contents of Model.

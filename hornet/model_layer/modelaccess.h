#pragma once

#include "editormodel.h"
#include "imodelaccess_readwrite.h"
#include "numbermodel.h"
#include "windowmodel.h"

/**
 * @brief The ModelAccess class
 *
 * Gives access to models.
 *
 * If the calling class has the interface IModelAccessRead included, it has reading access only.
 * Otherwise the interface IModelAccessReadWrite is needed.
 *
 * In this design Control has reading right, Services have reading and writing rights.
 */
class ModelAccess : public IModelAccessReadWrite {
public:
    ModelAccess();

    // first const: the caller will only get read-only access to what this numberModel() returns.
    // last const: this numberModel() will not modify ModelAccess while running.
    const NumberModel& getNumberModel() const override; // read-only

    // Overloaded numberModel(), allows writing access too.
    NumberModel& getNumberModel() override; // read-write

    // Returns WindowModel, as read-only.
    const WindowModel& getWindowModel() const override;

    // Returns WindowModel, as read-write.
    WindowModel& getWindowModel() override;

    // Returns as read-only.
    const EditorModel &getEditorModel() const override;

    // Returns as read-write.
    EditorModel &getEditorModel() override;

private:
    NumberModel m_numberModel;
    WindowModel m_windowModel;
    EditorModel m_editorModel;
};

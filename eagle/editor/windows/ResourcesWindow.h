//
// Created by Novak on 13/08/2019.
//

#ifndef EAGLE_RESOURCESWINDOW_H
#define EAGLE_RESOURCESWINDOW_H

#include "eagle/editor/windows/EditorWindow.h"

#include <boost/filesystem.hpp>

EG_EDITOR_BEGIN

class ResourcesWindow : public EditorWindow {
public:
    ResourcesWindow();
    virtual ~ResourcesWindow();
protected:
    virtual void handle_update() override;

private:

    void show_directory(const std::vector<boost::filesystem::path> &files,
                        std::vector<boost::filesystem::path>::iterator &it, bool foldout);

private:
    bool m_pathSelected = false;
    std::string m_path;

};

EG_EDITOR_END

#endif //EAGLE_RESOURCESWINDOW_H
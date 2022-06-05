#ifndef DIALOGEDITROUTES_H
#define DIALOGEDITROUTES_H

#include <QDialog>

class Route;

namespace Ui {
class DialogEditRoutes;
}

class DialogEditRoutes : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditRoutes(QWidget *parent = nullptr);
    ~DialogEditRoutes();

    void InitDialog(const std::vector<Route> &_routes);
    void ChangeRoutes(std::vector<Route> &_routes);

private:
    Ui::DialogEditRoutes *ui;
};

#endif // DIALOGEDITROUTES_H

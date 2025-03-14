#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_VirtualMemorySimulator.h"
#include "VirtualMemoryManager.h"

class VirtualMemorySimulator : public QMainWindow
{
    Q_OBJECT

public:
    VirtualMemorySimulator(QWidget *parent = nullptr);
    ~VirtualMemorySimulator();
    void highlightPage(QTableWidget* table, int pageNumber, int targetRow, int targetCol, int rowsPerPage, int durationMs);
    void updatePhysicalMemoryTable(int frameNumber, const std::vector<unsigned char>& frameData);
    void highlightRow(QTableWidget* table, int row, int durationMs);
    bool ok;
    //void updateTLB(int pageNumber);
private:
    Ui::VirtualMemorySimulatorClass ui;
    VirtualMemoryManager* virtualMemoryManager;
    int ramSize;
    int pageSize;
    int bytesPerRow = 8;
    int virtualAddressSpace = 4096;
    std::string replacementPolicy;
    int numPages;
    int numPagesRAM;
    int rowsPerPage;

    int currentStep;

    int pageNumber;
    int frameNumber;
    int offset;
    unsigned char byteToWrite;
    int physicalAddress;
private slots:
    void on_generateButton_clicked(); 
    void on_readButton_clicked();
    void on_writeButton_clicked();
    void on_nextButton_clicked();
};

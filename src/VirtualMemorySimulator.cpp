#include "VirtualMemorySimulator.h"
#include <QMessageBox>
#include <QTimer>
#include <cmath>
#include "VirtualMemoryManager.h"
#include <iostream>
#include <QDebug>

VirtualMemorySimulator::VirtualMemorySimulator(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

VirtualMemorySimulator::~VirtualMemorySimulator()
{}

bool writeOperation;

void VirtualMemorySimulator::on_generateButton_clicked()
{   
    ramSize = ui.comboBox->currentText().toInt();  // RAM size in Bytes
    pageSize = ui.comboBox_2->currentText().toInt();
    bytesPerRow = 8; // 8 bytes per row
    virtualAddressSpace = 4096;
    numPages = virtualAddressSpace / pageSize;
    numPagesRAM = ramSize / pageSize;
    replacementPolicy = ui.comboBox_3->currentText().toStdString();
    qDebug() << "replacement policy:" << replacementPolicy;
    //virtualMemoryManager instantiation 
    virtualMemoryManager = new VirtualMemoryManager(ramSize, pageSize, numPages, replacementPolicy);
    qDebug() << "replacement policy 1:" << virtualMemoryManager->physicalMemory.replacementPolicy;

    //Physical Memory Table
    QTableWidget* table = ui.tablePhysicalMemory;

    rowsPerPage = pageSize / bytesPerRow; // Rows per page
    int totalRows = numPagesRAM * rowsPerPage;   // Total rows

    table->setRowCount(totalRows);
    table->setColumnCount(10); // Columns: PageFrame + Start Addr + 8 bytes
    table->verticalHeader()->hide();
    table->setHorizontalHeaderLabels({ "PageFrame Nr", "Start Addr", "", "", "", "", "", "", "", "" });
    table->setStyleSheet("QTableWidget { background-color: #000000; color: white; }");

    for (int page = 0; page < numPagesRAM; ++page) {
        for (int rowOffset = 0; rowOffset < rowsPerPage; ++rowOffset) {
            int currentRow = page * rowsPerPage + rowOffset;

            if (rowOffset == 0) {
                table->setItem(currentRow, 0, new QTableWidgetItem(QString::number(page)));
            }
            else {
                table->setItem(currentRow, 0, new QTableWidgetItem(""));
            }

            int startAddress = page * pageSize + rowOffset * bytesPerRow;
            QString address = QString("0x%1").arg(QString::number(startAddress, 16).toUpper());
            table->setItem(currentRow, 1, new QTableWidgetItem(address));

            for (int col = 2; col <= 9; ++col) {
                QTableWidgetItem* byteItem = new QTableWidgetItem("-");
                table->setItem(currentRow, col, byteItem);
            }
        }
    }

    table->resizeColumnsToContents();

    //Virtual Addresses Table
    QTableWidget* table1 = ui.tableVirtualAddressSpace;
    totalRows = numPages * rowsPerPage;

    table1->setRowCount(totalRows);
    table1->setColumnCount(10); // Columns: VPage Number + Start Address + 8 bytes
    table1->verticalHeader()->hide();
    table1->setHorizontalHeaderLabels({"VPage Nr", "Start Addr", "", "" , "" , "" , "" , "" , "" , ""});
    table1->setStyleSheet("QTableWidget { background-color: #000000; color: white; }");

    for (int page = 0; page < numPages; ++page) {
        for (int rowOffset = 0; rowOffset < rowsPerPage; ++rowOffset) {
            int currentRow = page * rowsPerPage + rowOffset;

            if (rowOffset == 0) { // Show VPage Nr 
                table1->setItem(currentRow, 0, new QTableWidgetItem(QString::number(page)));
            }
            else {
                table1->setItem(currentRow, 0, new QTableWidgetItem("")); 
            }

            int startAddress = page * pageSize + rowOffset * bytesPerRow;
            QString address = QString("0x%1").arg(QString::number(startAddress, 16).toUpper());
            table1->setItem(currentRow, 1, new QTableWidgetItem(address));

            for (int col = 2; col <= 9; ++col) {
                int index = page * pageSize + rowOffset * bytesPerRow + (col - 2);
                int value = virtualMemoryManager->virtualAddressSpace.addresses[index];
                QString valueHex = QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
                table1->setItem(currentRow, col, new QTableWidgetItem(valueHex));
            }
        }
    }
    table1->resizeColumnsToContents();

    //Page Table
    QTableWidget* table2 = ui.tablePageTable;
    table2->setRowCount(numPages);
    table2->setColumnCount(6);
    table2->verticalHeader()->hide();
    table2->setHorizontalHeaderLabels({ "VPage Nr", "R", "M", "P", "P/A", "Page Frame Nr"});
    table2->setStyleSheet("QTableWidget { background-color: #000000; color: white; }");

    for (int i = 0; i < numPages; ++i) {
        table2->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
        table2->setItem(i, 1, new QTableWidgetItem(QString::number(0)));
        table2->setItem(i, 2, new QTableWidgetItem(QString::number(0)));
        table2->setItem(i, 3, new QTableWidgetItem(QString::number(0)));
        table2->setItem(i, 4, new QTableWidgetItem(QString::number(0)));
        table2->setItem(i, 5, new QTableWidgetItem("-"));
    }
    table2->resizeColumnsToContents();
  
    //TLB Table
    QTableWidget* table3 = ui.tableTLB;
    table3->setRowCount(6);
    table3->setColumnCount(6);
    table3->verticalHeader()->hide();
    table3->setHorizontalHeaderLabels({ "VPage Nr", "R", "M", "P", "P/A", "Page Frame Nr" });
    table3->setStyleSheet("QTableWidget { background-color: #000000; color: white; }");

    for (int i = 0; i < numPages; ++i) {
        table3->setItem(i, 0, new QTableWidgetItem("-"));
        table3->setItem(i, 1, new QTableWidgetItem(QString::number(0)));
        table3->setItem(i, 2, new QTableWidgetItem(QString::number(0)));
        table3->setItem(i, 3, new QTableWidgetItem(QString::number(0)));
        table3->setItem(i, 4, new QTableWidgetItem(QString::number(0)));
        table3->setItem(i, 5, new QTableWidgetItem("-"));
    }
    table3->resizeColumnsToContents();
    currentStep = -2;
}

void VirtualMemorySimulator::on_readButton_clicked()
{   
    ok = false;
    ui.messageLog->clear();
    writeOperation = false;
    QString readAddress = ui.readAddress->text();
    bool ok;
    unsigned int address = readAddress.toUInt(&ok, 16); // hex in int

    if (!ok || address > 0xFFFFFFFF) {
        QMessageBox::warning(this, "Invalid Input", "The read address is not correct!");
        return;
    }
    int translatedAddress = virtualMemoryManager->translateAddress(address);
    pageNumber = (translatedAddress >> 12) & 0x3F; // 0x3F = 111111 
    QTableWidget* table = ui.tableVirtualAddressSpace;

    offset = translatedAddress & 0xFFF; // 0xFFF = 111111111111 

    QString pageNumberBinary = QString::number(pageNumber, 2).rightJustified(6, '0'); 
    QString offsetBinary = QString::number(offset, 2).rightJustified(12, '0'); 

    int pageStartRow = pageNumber * rowsPerPage;
    int row = pageStartRow + (offset / 8);
    int col = 2 + (offset % 8);
    highlightPage(table, pageNumber, row, col, rowsPerPage, 4000);

    QString message = QString("Adresa este descompusa in:\n PageNumber: %1; Offset: %2")
        .arg(pageNumberBinary)
        .arg(offsetBinary);

    ui.messageLog->append(message);
    currentStep = -1;

}

void VirtualMemorySimulator::on_writeButton_clicked()
{
    ok = false;
    ui.messageLog->clear();
    writeOperation = true; 
    QString writeAddress = ui.writeAddress->text();
    QString writeData = ui.writeByte->text();

    bool okAddress, okData;
    unsigned int address = writeAddress.toUInt(&okAddress, 16); // hex to int
    unsigned int data = writeData.toUInt(&okData, 16);        

    if (!okAddress || !okData || address > 0xFFFFFFFF || data > 0xFF) {
        QMessageBox::warning(this, "Invalid Input", "The write address or data is not correct!");
        return;
    }

    byteToWrite = static_cast<unsigned char>(data);

    int translatedAddress = virtualMemoryManager->translateAddress(address);
    pageNumber = (translatedAddress >> 12) & 0x3F; 
    offset = translatedAddress & 0xFFF;          
    QString pageNumberBinary = QString::number(pageNumber, 2).rightJustified(6, '0');
    QString offsetBinary = QString::number(offset, 2).rightJustified(12, '0');

    QTableWidget* table = ui.tableVirtualAddressSpace;

    int pageStartRow = pageNumber * rowsPerPage;
    int row = pageStartRow + (offset / 8);
    int col = 2 + (offset % 8);

    highlightPage(table, pageNumber, row, col, rowsPerPage, 4000);

    QString message = QString("Adresa este descompusa in:\n PageNumber: %1; Offset: %2")
        .arg(pageNumberBinary)
        .arg(offsetBinary);
    ui.messageLog->append(message);
    currentStep = -1;
}


void VirtualMemorySimulator::on_nextButton_clicked() {
    qDebug() << "Current Step:" << currentStep;  // Log the value of currentStep

    switch (currentStep) {
    case -1: {
        physicalAddress = virtualMemoryManager->tlb.getPhysicalAddress(pageNumber, offset, writeOperation);
        if (physicalAddress == -1)
        {
            QString message = QString("TLB miss. Pagina se va cauta in Page Table");
            ui.messageLog->append(message);
            currentStep = 0;
            break;
        }
        else if (physicalAddress == -2)
        {
            QString message = QString("Pagina poate fi doar citita!");
            ui.messageLog->append(message);
            currentStep = -2;
            break;
        }
        else if (physicalAddress == -3)
        {
            QString message = QString("TBL hit! Pagina nu este prezenta in RAM!");
            ui.messageLog->append(message);
            currentStep = 1;
            break;

        }
        else {
            int updatePageTable = virtualMemoryManager->pageTable.getPhysicalAddress(pageNumber, offset, writeOperation);
            //qDebug() << "Valoare referenced:" << virtualMemoryManager->tlb.getEntryValues(pageNumber)[1];
            //qDebug() << "modified:" << virtualMemoryManager->tlb.getEntryValues(pageNumber)[2];
            //qDebug() << "Valoare referenced pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[1];
            //qDebug() << "modified pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[2];
            QString message = QString("TLB hit! Adresa fizica: 0x%1")
                .arg(QString::number(physicalAddress, 16).toUpper());
            ui.messageLog->append(message);
            currentStep = 1;
            break;

        }
    }
    case 0: {
        physicalAddress = virtualMemoryManager->pageTable.getPhysicalAddress(pageNumber, offset, writeOperation);
        if (physicalAddress == -1)
        {
            QString message = QString("Page Fault. Pagina trebuie incarcata din memoria secundara");
            ui.messageLog->append(message);
            currentStep = 1;
            break;
        }
        else if (physicalAddress == -2)
        {
            QString message = QString("Pagina poate fi doar citita!");
            ui.messageLog->append(message);
            currentStep = -2;
            break;
        }
        else if (physicalAddress == -3)
        {
            QString message = QString("Page Table hit! Pagina nu este prezenta in RAM!");
            ui.messageLog->append(message);
            currentStep = 1;
            break;

        }
        else {
            QString message = QString("Page Table hit!  Adresa fizica: 0x%1")
                .arg(QString::number(physicalAddress, 16).toUpper());
            ui.messageLog->append(message);
            currentStep = 1;
            break;

        }
    }
    case 1: {
        //daca physicaladdress >=0 s-a gasit, altfel trebuie incarcat in RAM 
        if (physicalAddress >= 0)
        {
            //s-a gasit pagina in TLB sau PageTable
            //cand TBL hit se adauga in rand gol in tabel??
            if (writeOperation == true) {
                virtualMemoryManager->physicalMemory.write(frameNumber, offset, byteToWrite);

                int startRow = frameNumber * rowsPerPage;
                int row = startRow + (offset / 8);
                int col = 2 + (offset % 8);

                QString dataHex = QString("%1").arg(byteToWrite, 2, 16, QChar('0')).toUpper();
                QTableWidgetItem* item = new QTableWidgetItem(dataHex);
                ui.tablePhysicalMemory->setItem(row, col, item);
                highlightPage(ui.tablePhysicalMemory, frameNumber, row, col, rowsPerPage, 4000);

                QString message = QString("Valoarea la adresa %1 a fost actualizata cu %2.")
                    .arg(physicalAddress)
                    .arg(dataHex);
                ui.messageLog->append(message);
                //actualizam modified si referenced bit
            }
            else {
                unsigned char data = virtualMemoryManager->physicalMemory.read(frameNumber, offset);
                int startRow = frameNumber * rowsPerPage;
                int row = startRow + (offset / 8);
                int col = 2 + (offset % 8);

                highlightPage(ui.tablePhysicalMemory, frameNumber, row, col, rowsPerPage, 4000);


                QString dataHex = QString("%1").arg(data, 2, 16, QChar('0')).toUpper();
                QString message = QString("Valoarea %1 a fost citita de la adresa %2.")
                    .arg(dataHex)
                    .arg(physicalAddress);
                ui.messageLog->append(message);

            }
            virtualMemoryManager->tlb.update(pageNumber, writeOperation, 0);
            virtualMemoryManager->pageTable.update(pageNumber, writeOperation, 0);
            qDebug() << "Valoare referenced pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[1];
            qDebug() << "modified pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[2];
            ok = false;
            currentStep = 2;
            break;
            //actualizam TLB si Page Table in ui


        }
        else {
            //trebuie se incarcam pagina din memoria secundara si sa actualizam TLB si PageTable;
            qDebug() << "Trebuie incarcata pagina in Ram; ifFull : " << virtualMemoryManager->physicalMemory.isFull();
            if (virtualMemoryManager->physicalMemory.isFull() != true)
            {
                //daca RAM mai are spatiu liber
                frameNumber = virtualMemoryManager->physicalMemory.addPageFrame(pageNumber);
                QTableWidget* table = ui.tablePhysicalMemory;

                int startRow = frameNumber * rowsPerPage;

                //actualizam tabelul cu valorile noi
                for (int rowOffset = 0; rowOffset < rowsPerPage; ++rowOffset) {
                    int currentRow = startRow + rowOffset;
                    int startAddress = frameNumber * pageSize + rowOffset * 8;

                    for (int col = 2; col <= 9; ++col) {
                        int offset = (rowOffset * 8) + (col - 2);
                        unsigned char data = virtualMemoryManager->physicalMemory.read(frameNumber, offset);
                        QString dataHex = QString("%1").arg(data, 2, 16, QChar('0')).toUpper();
                        table->setItem(currentRow, col, new QTableWidgetItem(dataHex));
                    }
                }

                table->resizeColumnsToContents();
                int pageStartRow = frameNumber * rowsPerPage;
                int row = pageStartRow + (offset / 8);
                int col = 2 + (offset % 8);

                highlightPage(table, frameNumber, row, col, rowsPerPage, 4000);

                QString message = QString("Pagina %1 a fost incarcata la Page frame-ul %2.")
                    .arg(pageNumber)
                    .arg(frameNumber);
                ui.messageLog->append(message);
                //actualizam TLB si PageTable??
                //physicalAddress = virtualMemoryManager->pageTable.getPhysicalAddress(pageNumber, offset, writeOperation); //??
                //qDebug() << "physicalAddress dupa adaugare  " << virtualMemoryManager->physicalMemory.isFull();
                //physicalAddress = 1;
                currentStep = 2;
                ok = true;
                break;

            }
            else {
                //daca RAM e plina 
                qDebug() << "RAM plina, replacemwent policy:" << replacementPolicy;
                int evictedPageNumber = virtualMemoryManager->physicalMemory.removePageFrame();

                //////
                if (virtualMemoryManager->tlb.findIndex(evictedPageNumber) != -1)
                {
                    //update tlb sa fie absent
                    virtualMemoryManager->tlb.update(evictedPageNumber, 0, 1);
                    QTableWidget* table = ui.tableTLB;
                    int index = virtualMemoryManager->tlb.findIndex(evictedPageNumber);
                    qDebug() << "INDEX TABEL case 2:" << index;
                    std::vector<int> entry(6);
                    entry = virtualMemoryManager->tlb.getEntryValues(evictedPageNumber);
                    if (index >= 0) {
                        for (int col = 0; col < 6; ++col) {
                            table->setItem(index, col, new QTableWidgetItem(QString::number(entry[col])));
                        }
                    }
                    table->resizeColumnsToContents();
                    highlightRow(table, index, 4000);
                }
                ///////
                int evictedFrameNumber = virtualMemoryManager->pageTable.getPageFrameNumber(evictedPageNumber);

                if (virtualMemoryManager->pageTable.isModified(evictedPageNumber)) {
                    virtualMemoryManager->physicalMemory.writeToSecondaryMem(evictedFrameNumber);
                    ui.messageLog->append(QString("Pagina %1 a fost incarcata in memoria secundara.").arg(evictedPageNumber));
                }

                virtualMemoryManager->pageTable.setMapping(evictedPageNumber, -1, false);
                virtualMemoryManager->pageTable.update(evictedPageNumber, 0, 1); /////

                //update page table for evicted page
                QTableWidget* table = ui.tablePageTable;
                //std::vector<int> entry(6);
                //entry = virtualMemoryManager->pageTable.getEntryValues(evictedPageNumber);
                table->setItem(evictedPageNumber, 0, new QTableWidgetItem(QString::number(0)));
                table->setItem(evictedPageNumber, 1, new QTableWidgetItem(QString::number(0)));
                table->setItem(evictedPageNumber, 2, new QTableWidgetItem(QString::number(0)));
                table->setItem(evictedPageNumber, 3, new QTableWidgetItem(QString::number(0)));
                table->setItem(evictedPageNumber, 4, new QTableWidgetItem(QString::number(0)));
                table->setItem(evictedPageNumber, 5, new QTableWidgetItem(QString::number(-1)));
                table->resizeColumnsToContents();
                highlightRow(table, evictedPageNumber, 4000);

                frameNumber = virtualMemoryManager->physicalMemory.addPageFrame(pageNumber); //
                virtualMemoryManager->pageTable.setMapping(pageNumber, frameNumber, /* protection */ false);

                table = ui.tablePhysicalMemory;
                int startRow = frameNumber * rowsPerPage;

                // Update the table with the new page values
                for (int rowOffset = 0; rowOffset < rowsPerPage; ++rowOffset) {
                    int currentRow = startRow + rowOffset;
                    int startAddress = frameNumber * pageSize + rowOffset * 8;

                    for (int col = 2; col <= 9; ++col) {
                        int offset = (rowOffset * 8) + (col - 2);
                        unsigned char data = virtualMemoryManager->physicalMemory.read(frameNumber, offset);
                        QString dataHex = QString("%1").arg(data, 2, 16, QChar('0')).toUpper();
                        table->setItem(currentRow, col, new QTableWidgetItem(dataHex));
                    }
                }

                table->resizeColumnsToContents();
                int pageStartRow = frameNumber * rowsPerPage;
                int row = pageStartRow + (offset / 8);
                int col = 2 + (offset % 8);

                highlightPage(table, frameNumber, row, col, rowsPerPage, 4000);

                QString message = QString("Pagina %1 a fost incarcata la Page frame-ul %2. Pagina %3 a fost eliminata.")
                    .arg(pageNumber)
                    .arg(frameNumber)
                    .arg(evictedPageNumber);
                ui.messageLog->append(message);

                // Update the TLB with the new mapping
                qDebug() << "AAAA:";
               // virtualMemoryManager->tlb.update(pageNumber, frameNumber);
                //virtualMemoryManager->tlb.setMapping()
                ok = true;
                qDebug() << "BBBB:";

                currentStep = 2;
                break;
            }

        }
    }
    case 2: {
        //actualizam TLB si Page Frame
        //!!in caz de tlb hit se actualizeaza doar tlb table??
        //in caz de tlb hit sau pageTable hit tot ajunge aici si se schimba entryurile (ar trebui doar referenced si/sau modified
        if (virtualMemoryManager->tlb.findIndex(pageNumber) == -1)
        {
            virtualMemoryManager->tlb.setMapping(pageNumber, frameNumber, 0);
        }
        if (ok)
        {
            //virtualMemoryManager->tlb.setMapping(pageNumber, frameNumber, 0);
            // 
            //+ cazu in care TLB este plin? (se face in setMapping)
            virtualMemoryManager->pageTable.setMapping(pageNumber, frameNumber, 0);
        }
        //actualizam tabelele
        //TLB
        QTableWidget* table = ui.tableTLB;
        int index = virtualMemoryManager->tlb.findIndex(pageNumber);
        qDebug() << "INDEX TABEL case 2:" << index;
        std::vector<int> entry(6);
        entry = virtualMemoryManager->tlb.getEntryValues(pageNumber);
        if (index >= 0) {
            for (int col = 0; col < 6; ++col) {
                table->setItem(index, col, new QTableWidgetItem(QString::number(entry[col])));
            }
        }
        table->resizeColumnsToContents();
        highlightRow(table, index, 4000);
        //Page Table
        qDebug() << "Valoare referenced pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[1];
        qDebug() << "modified pagetable:" << virtualMemoryManager->pageTable.getEntryValues(pageNumber)[2];
        table = ui.tablePageTable;
        entry = virtualMemoryManager->pageTable.getEntryValues(pageNumber);
        table->setItem(pageNumber, 0, new QTableWidgetItem(QString::number(entry[0])));
        table->setItem(pageNumber, 1, new QTableWidgetItem(QString::number(entry[1])));
        table->setItem(pageNumber, 2, new QTableWidgetItem(QString::number(entry[2])));
        table->setItem(pageNumber, 3, new QTableWidgetItem(QString::number(entry[3])));
        table->setItem(pageNumber, 4, new QTableWidgetItem(QString::number(entry[4])));
        table->setItem(pageNumber, 5, new QTableWidgetItem(QString::number(entry[5])));
        table->resizeColumnsToContents();
        highlightRow(table, pageNumber, 4000);

        QString message = QString("S-au actualizat TLB si Page Table");
        ui.messageLog->append(message);
        if (ok)
        {
            //physicalAddress = 1;
            currentStep = -1;
            break;
        }
        currentStep = -2;
        break;
    }
    case -2: {
        QString message = QString("Done!");
        ui.messageLog->append(message);
    }
    }
  
}

void VirtualMemorySimulator::updatePhysicalMemoryTable(int frameNumber, const std::vector<unsigned char>& frameData)
{
    QTableWidget* table = ui.tablePhysicalMemory;
    
}

void VirtualMemorySimulator::highlightPage(QTableWidget* table, int pageNumber, int elemRow, int elemCol, int rowsPerPage, int durationMs)
{
    int pageStartRow = pageNumber * rowsPerPage;

    for (int row = pageStartRow; row < pageStartRow + rowsPerPage; ++row) {
        for (int col = 2; col < table->columnCount(); ++col) {
            QTableWidgetItem* item = table->item(row, col);

            if (row == elemRow && col == elemCol) {
                item->setBackground(QBrush(QColor(120, 0, 0))); //dark red
            }
            else {
                item->setBackground(QBrush(QColor(Qt::red)));
            }
        }
    }

    QTimer::singleShot(durationMs, [table, pageStartRow, rowsPerPage]() {
        for (int row = pageStartRow; row < pageStartRow + rowsPerPage; ++row) {
            for (int col = 2; col < table->columnCount(); ++col) {
                QTableWidgetItem* item = table->item(row, col);
                item->setBackground(QBrush(QColor(Qt::black)));
            }
        }
        });
}

void VirtualMemorySimulator::highlightRow(QTableWidget* table, int row, int durationMs) {
    int columnCount = table->columnCount();

    for (int col = 0; col < columnCount; ++col) {
        QTableWidgetItem* item = table->item(row, col);
        if (item) {
            item->setBackground(QBrush(QColor(Qt::red)));
        }
    }

    QTimer::singleShot(durationMs, [table, row, columnCount]() {
        for (int col = 0; col < columnCount; ++col) {
            QTableWidgetItem* item = table->item(row, col);
            if (item) {
                item->setBackground(QBrush(QColor(Qt::black)));
            }
        }
        });
}

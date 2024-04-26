#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TColor.h>


using namespace std;

void correlations()
{
    auto myTree = new TChain("Hlt2DetachedJpsiTuple/Tuple");
    myTree->Add("/home/javeser/Desktop/TFM/Lb2_JpsiLambda_vtx.root");

    std::vector<TString> names;
    std::vector<TString> filtered_names;
    std::vector<float> minCuts = {0.9996,0,0,0,0,0,0,0,0,0.9999,0,0,0,1000,0,0,-40,-40,0,0,0,0,0}; // Valores mínimos de los cortes
    std::vector<float> maxCuts = {1,0.2,4,50,400000,30000,25,20000,1200,1,15,
        100,50,3000,15000,10,60,40,400,200000,15000,50000,4000}; // Valores máximos de los cortes
    bool cortes; // Just to compile with cuts or not
    bool selec_var = false;

    for (auto i : myTree->GetListOfBranches()[0]) {
        names.push_back(i->GetName());
    }

    
    if(selec_var == true){
        for(size_t i = 0; i < names.size(); i++){
//            Check and don't select de variables that contains MOTHER or TRUE
            TString var_t = names[i];
            if( var_t.Index("TRUE")==kNPOS && var_t.Index("MOTHER")==kNPOS){
                filtered_names.push_back(var_t);
                cout << "Comprobando variables:" << var_t << endl;
            }
        }

        
    }else{
        for (auto& var : names) {
            // Comprueba si la variable contiene las palabras "TRUE" o "MOTHER"
            if (var.EqualTo("Jpsi_PT") || var.EqualTo("Lambda0_END_VRHO") || var.EqualTo("Lambda0_BPVDIRA") || var.EqualTo("Lambda0_BPVIP") || var.EqualTo("Lambda0_BPVIPCHI2") || var.EqualTo("Lambda0_MAXSDOCA") || var.EqualTo("Lambda0_PT") || var.EqualTo("Lambda0_CHI2") || var.EqualTo("Lambda0_MASS") ||  var.EqualTo("Lb_BPVDIRA") || var.EqualTo("Lb_BPVIP") || var.EqualTo("Lb_BPVVDRHO") || var.EqualTo("Lb_MAXSDOCA") ||  var.EqualTo("Lb_P") || var.EqualTo("Lb_PT") || var.EqualTo("Lb_CHI2") || var.EqualTo("p_plus_PID_P") || var.EqualTo("p_plus_PID_K") || var.EqualTo("p_plus_MINIP") || var.EqualTo("p_plus_P")|| var.EqualTo("p_plus_PT")|| var.EqualTo("pi_minus_P")|| var.EqualTo("pi_minus_PT")) {
                // Si no contiene ninguna de las palabras, se añade la variable al vector.
                filtered_names.push_back(var);
                cout << "Comprobando variable: " << var << endl;
            }
        }
    }

    


    float tam = filtered_names.size();

    cout << "El tamaño del vector de variables es de: " << tam << endl;
    cout << "Se desean realizar cortes a las variables?(yes->1/no->0) " << endl;
    cin >> cortes;

    const int rows = 3;
    const int cols = 3;
    const int max_plots_per_page = rows * cols;

    // contadores del canvas y la página
    int canv_count = 0;
    int page_count = 0;
    int hist_count = 0;

    // Configuración de colores
    gStyle->SetPalette(kRainBow);

    if(cortes == true){
         // loop para crear todos los histogramas
    for (int i = 0; i < filtered_names.size(); ++i) {
        auto var1 = filtered_names[i];

        for (int j = i + 1; j < filtered_names.size(); ++j) {
            auto var2 = filtered_names[j];

            auto histo = new TH2F((var1 + "_vs_" + var2).Data(), (var1 + "_vs_" + var2).Data(), 100, minCuts[i],maxCuts[i], 100, minCuts[j], maxCuts[j]);

            
            myTree->Project((var1 +"_vs_" + var2).Data(), TString::Format("%s:%s", var2.Data(), var1.Data(), "Lb_BKGCAT!=0"));
            histo->GetXaxis()->SetTitle(var1);
            histo->GetYaxis()->SetTitle(var2);

            // create a new canvas and page every max_plots_per_page histograms
            if (hist_count % max_plots_per_page == 0) {
                canv_count++;
                auto c1 = new TCanvas(("c" + to_string(canv_count)).c_str(), ("c" + to_string(canv_count)).c_str(), 1600, 1200);
                c1->Divide(cols, rows);
                page_count++;
            }

            // dibuja el histograma en el canvas
            auto c1 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(("c" + to_string(canv_count)).c_str());
            c1->cd(hist_count % max_plots_per_page + 1);
            histo->SetStats(false);  // Desactiva la tabla de estadísticas
            histo->Draw("COLZ");

            
            // Agrega el índice de correlación en la esquina superior izquierda del histograma
                        double correlation = histo->GetCorrelationFactor();
                        TText* text = new TText(0.1, 0.95, TString::Format("Correlation: %.3f", correlation));
                        text->SetNDC();
                        text->SetTextSize(0.04);
                        text->SetTextColor(kRed);
                        text->Draw();

            hist_count++;

            // guarda el canvas y pasa a la siguiente página si se han generado max_plots_per_page histogramas o es el último
            if (hist_count % max_plots_per_page == 0 || hist_count == (filtered_names.size() * (filtered_names.size() - 1) / 2)) {
                c1->Print(("correlations_page_" + to_string(page_count) + ".pdf").c_str());
            }
            
        }
    } cout << "Se han realizado los cortes correctamente" << endl;
    }
    else
    {
        for (int i = 0; i < filtered_names.size(); ++i) {
        auto var1 = filtered_names[i];

        for (int j = i + 1; j < filtered_names.size(); ++j) {
            auto var2 = filtered_names[j];

            auto histo = new TH2F((var1 + "_vs_" + var2).Data(), (var1 + " vs " + var2).Data(), 100, myTree->GetMinimum(var1), myTree->GetMaximum(var1), 100, myTree->GetMinimum(var2), myTree->GetMaximum(var2));
            myTree->Project((var1 + "_vs_" + var2).Data(), TString::Format("%s:%s", var2.Data(), var1.Data()));
            histo->GetXaxis()->SetTitle(var1);
            histo->GetYaxis()->SetTitle(var2);

            // create a new canvas and page every max_plots_per_page histograms
            if (hist_count % max_plots_per_page == 0) {
                canv_count++;
                auto c1 = new TCanvas(("c" + to_string(canv_count)).c_str(), ("c" + to_string(canv_count)).c_str(), 1600, 1200);
                c1->Divide(cols, rows);
                page_count++;
            }

            // dibuja el histograma en el canvas
            auto c1 = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(("c" + to_string(canv_count)).c_str());
            c1->cd(hist_count % max_plots_per_page + 1);
            histo->SetStats(false);  // Desactiva la tabla de estadísticas
            histo->Draw("COLZ");

            
            // Agrega el índice de correlación en la esquina superior izquierda del histograma
                        double correlation = histo->GetCorrelationFactor();
                        TText* text = new TText(0.1, 0.95, TString::Format("Correlation: %.3f", correlation));
                        text->SetNDC();
                        text->SetTextSize(0.04);
                        text->SetTextColor(kRed);
                        text->Draw();

            hist_count++;

            // guarda el canvas y pasa a la siguiente página si se han generado max_plots_per_page histogramas o es el último
            if (hist_count % max_plots_per_page == 0 || hist_count == (filtered_names.size() * (filtered_names.size() - 1) / 2)) {
                c1->Print(("correlations_page_" + to_string(page_count) + ".pdf").c_str());
            }
        }
        } cout << "No se han realizado los cortes, como se estableció" << endl;
        
    }

	cout << "Lista de varaibles con sus correlaciones:" << endl;

    	for (int i = 0; i < filtered_names.size(); ++i) {
        auto var1 = filtered_names[i];

        	for (int j = i + 1; j < filtered_names.size(); ++j) {
           	 auto var2 = filtered_names[j];

            	auto histo2 = new TH2F((var1 + "_vs_" + var2).Data(), (var1 + " vs " + var2).Data(), 100, myTree->GetMinimum(var1), myTree->GetMaximum(var1), 100, myTree->GetMinimum(var2), myTree->GetMaximum(var2));
            	myTree->Project((var1 + "_vs_" + var2).Data(), TString::Format("%s:%s", var2.Data(), var1.Data()));
            	histo2->GetXaxis()->SetTitle(var1);
            	histo2->GetYaxis()->SetTitle(var2);

            	// Agrega el índice de correlación en la esquina superior izquierda del histograma
                double correlation = histo2->GetCorrelationFactor();
                
		cout << var1 << " vs " << var2 << ": " << correlation << endl;
        	}
        
      }  
}


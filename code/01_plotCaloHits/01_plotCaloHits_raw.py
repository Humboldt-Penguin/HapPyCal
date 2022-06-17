import ROOT as root
import os.path
import numpy as np
import time
import copy

def main():
    
    # user input path
    finPath = "02_data/calorimeter/Elog2341/run12130_calo_qdc_tdc_raw.txt"
    plotPersistance = False
    
    t0 = time.time()

    # read/load data from file
    TDC, QDC = load_TQDC(finPath)
    
    # make and save 8x8 histograms
    resolution = 10
    
    QDC_canv, QDC_hist = make8x8("QDC", 0, 1000, resolution, QDC) # note that canv+hists must be returned so they don't immediately disappear
    QDC_canv.Print("raw_QDC_canv.pdf")
    
    TDC_canv, TDC_hist = make8x8("TDC", -100, 300, resolution, TDC)
    TDC_canv.Print("raw_TDC_canv.pdf")
    
    t1 = time.time()

    print(f"\nTotal runtime was {t1-t0:.2f} seconds.\n")
    
    if plotPersistance:
        input("Press enter to close...\n")
        
    

####################################################################################

def load_TQDC(finPath):
    basePath = "/media/sf_MUSE/"

    # load data from file
    fin = os.path.join(basePath, finPath)
    t0 = time.time()
    raw_data = np.loadtxt(fin, skiprows = 5)
    t1 = time.time()
    print(f"Data file read in {t1-t0:.2f} seconds.")

    # initialize formatted data array
    num_events = raw_data.shape[0]
    TDC = np.zeros((num_events+1, 8, 8))
    QDC = np.zeros((num_events+1, 8, 8))

    # format data to [event#][x][y]
    t0 = time.time()
    for event in raw_data:
        for x in range(8):
            TDC [ np.int_(event[0]) ] [x] [:] = event[16*x+1 : 16*x+17 : 2]
            QDC [ np.int_(event[0]) ] [x] [:] = event[16*x+2 : 16*x+17 : 2]
    t1 = time.time()
    print(f"Data file formatted to array in {t1-t0:.2f} seconds.")
    
    return TDC, QDC



####################################################################################
	
def make8x8(datatype, xMin, xMax, resolution, data):

    t0 = time.time()

    nBins = int(xMax / resolution)

    
    # make canvas
    data_canv = root.TCanvas(f"raw_{datatype}_canv", f"{datatype} Bars (counts v channel)")
    data_canv.Divide(8,8)
    
    # fill histograms
    data_hist = [root.TH1F()] * 65
    for x in range(8):
        for y in range(8):
        
            # get pad coordinates
            pad_x = x
            pad_y = -y + 7
            pad_index = pad_x + 8*pad_y + 1 # referenced by CD
        
        
            # fill histogram
            data_hist[pad_index] = root.TH1F(f"bar ({x},{y})", f"bar ({x},{y})", nBins, float(xMin), float(xMax))
            num_events = data.shape[0] - 1
            for event in range(1,num_events+1):
                data_hist[pad_index].Fill(data[event][x][y])
                
                

                        
                
            # pedestal correction (QDC only)
            if datatype == "QDC":
            
                ## get max bin index and value
                maxBinIndex = 0
                maxBinValue = data_hist[pad_index].GetBinContent(0)
            
                for i_bin in range(nBins):
                    if (data_hist[pad_index].GetBinContent(i_bin) > maxBinValue):
                        maxBinValue = data_hist[pad_index].GetBinContent(i_bin)
                        maxBinIndex = i_bin
            
                ## copy bins leftwards so max bin is at 0
                for i_bin in range(0, nBins - maxBinIndex + 1):
                    data_hist[pad_index].SetBinContent(i_bin, data_hist[pad_index].GetBinContent(i_bin + maxBinIndex))
                    
                ## erase remaining bins on the right edge
                for i_bin in range(nBins-maxBinIndex, nBins + 1):
                    data_hist[pad_index].SetBinContent(i_bin, 0)
                    
                ## recalculate stats
                data_hist[pad_index].PutStats(np.zeros(5))
                    
                    
            # get max value in entire dataset
            maxValue = np.amax(data)

                
                
            # format/draw a deep copy of this histogram
            data_hist[pad_index] = copy.deepcopy(data_hist[pad_index])
            data_hist[pad_index].SetMaximum(maxValue * 1.1)
            data_canv.cd(pad_index)
            if datatype == "QDC":
                data_canv.GetPad(pad_index).SetLogy()
            data_hist[pad_index].Draw()
    
    data_canv.SetCanvasSize(2000,2000)
    
    t1 = time.time()
    print(f"{datatype} histogram created in {t1-t0:.2f} seconds.")
    
    return data_canv, data_hist
    
####################################################################################

if __name__ == "__main__":
    main()

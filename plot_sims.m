clear
more off
Plotz=0 # Perf graphs
Convz=0 # Conv graphs
Unitz=0 # Perf 10,20,...,100
opt=0
# graphical properties

# Experiment consts
home_path_sim='C:\Users\Matusovsky\Desktop\Masters-UC\results\experiments\PerformanceStatic\24_Feb_2016\'
# Performance
wrc_path_sim='\\file\UsersY$\yma61\Home\Desktop\results\experiments\PerformanceStatic\18_May_2015\'
home_linux_path='/home/yakir/Workspace/msc/results/experiments/PerformanceStatic/17_Jun_2015/'
# Convergence
home_conv_path_sim='C:\Users\Matusovsky\Desktop\Masters-UC\results\experiments\PerformanceStatic\24_Feb_2016\'
# Convergence
home_unit_path_sim='C:\Users\Matusovsky\Desktop\Masters-UC\results\experiments\PerformanceStatic\10_Aug_2015\'

function setLabels(xl,tit)
  xlabel(xl)
  ylabel("V_T", "rotation", 0)
  legend("NCQRS", "DBP", "CAOS", "location", "northwest")
  grid("on")
  title(tit)
endfunction

function setLabels4B(tit)
  xlabel("E[H_1]")
  ylabel("V_T", "rotation", 0)
  legend("B=0.3", "B=0.5", "B=0.7", "B=0.9", "location", "northwest")
  grid("on")
  title(tit)
endfunction

function setLabelsMK1(tit)
  xlabel("PER")
  ylabel("V_T", "rotation", 0)
  legend("NCQRS (3,4)-firm", "DBP (3,4)-firm", "CAOS (3,4)-firm",
         "NCQRS (8,10)-firm", "DBP (8,10)-firm", "CAOS (8,10)-firm", 
         "location", "northwest")
  grid("on")
  title(tit)
endfunction

function setLabelsPConv(xl,tit)
  xlabel(xl)
  ylabel("sec", "rotation", 90)
  legend("(3,4)-firm", "(7,8)-firm", "(14,15)-firm", "location", "northeast")
  grid("on")
  title(tit)
endfunction

function setLabelsmkConv(xl,tit,per)
  xlabel(xl)
  ylabel("time slots", "rotation", 90)
  if (per == 0)
    legend("10 users", "15 users", "20 users", "location", "northwest")
  else
    legend("10 users", "15 users", "location", "northwest")
  endif
  grid("on")
  title(tit)
endfunction

function plotSimes(p, filez, titlez, nfigures, points, xs, firstFigure, pdist, xtype, pnext)
  RLQLine="-*"
  DBPLine=":o"
  CAOSLine="--+"
  RLQ2Line="-*"
  DBP2Line=":o"
  CAOS2Line="--+"  
  # assume points located in one file for pdist>0

  RLQ={} 
  DBP={}
  CAOS={}
  StY={}
  RLV={} 
  DBV={}
  CAOV={}
  
  line={RLQLine,CAOSLine,DBPLine}
  
  # for the case of convergence graphs with x-axis of (m,k)-firm
  # and multiple curves for different scenario (aka filez)
  if (xtype == 4)
    figure(1 + firstFigure)
    hold all;
    for i = 1:numel(filez)
      StY(i) = load("-ascii", char( strcat( p, filez{1,i}) ))
      plot(xs,cell2mat(StY(i)),line(i))
    endfor
    hold off;
    #plot(xs, cell2mat(StY(1)), RLQLine, xs, cell2mat(StY(2)), CAOSLine, xs, cell2mat(StY(3)), DBPLine) 
    setLabelsmkConv(pnext, titlez(1), pdist) # pdist used as conv type switch
    return
  endif
  
  if (xtype == 5)
    i = 1
    f = 0
    t = 1
    while(i <= numel(filez))
        figure(1 + f)
        for j = 1:points
          StY{i,j} = load("-ascii", char( strcat( p, filez{1,i}{j}) ))
          RLQ(j) = StY{i,j}(1)
          DBP(j) = StY{i,j}(2)
          CAOS(j) = StY{i,j}(3)  
          usleep(100000) 
        endfor
        i=i+1
        for j = 1:points
          StY{i,j} = load("-ascii", char( strcat( p, filez{1,i}{j}) ))
          RLV(j) = StY{i,j}(1)
          DBV(j) = StY{i,j}(2)
          CAOV(j) = StY{i,j}(3)   
          usleep(100000)
        endfor
        i=i+1
        f=f+1
      
        plot(xs,cell2mat(RLQ),RLQLine,xs,cell2mat(DBP),DBPLine,xs,cell2mat(CAOS),CAOSLine,
             xs,cell2mat(RLV),RLQ2Line,xs,cell2mat(DBV),DBP2Line,xs,cell2mat(CAOV),CAOS2Line)
        
        setLabelsMK1(titlez(t))
        t = t+1
    endwhile
    return
  endif
  
  # for this case the -dist determines state and offset within one file
  # while pnext defines offset of specific setting (e.g. which E[h1] to take)
  # pnext must be passed as array of offsets (wanted E[h1]'s for example)
  if (pdist < 0) 
    for f = 1:nfigures
      figure(f + firstFigure)
      usleep(200000)
      RLQ={} 
      DBP={}
      CAOS={}
      for i = 1:numel(filez)
        StY = load("-ascii", char( strcat( p, filez{1,i}) ))
        RLQ(i) = StY(i+pnext(f))
        DBP(i) = StY(i-pdist+pnext(f))
        CAOS(i) = StY(i-2*pdist+pnext(f))
      endfor
      plot(xs,cell2mat(RLQ),RLQLine,xs,cell2mat(DBP),DBPLine,xs,cell2mat(CAOS),CAOSLine)
      if (xtype == 3)
        setLabelsPConv("PER", titlez(f))
      else
        setLabels("PER", titlez(f))
      endif
    endfor
    return
  endif    
  
  for i = 1:nfigures
    if (pdist > 0)
      StY = load("-ascii", char( strcat( p, filez{1,i}) ))
      for j = 1:points
        RLQ(j) = StY(j)
        DBP(j) = StY(j+pdist)
        CAOS(j) = StY(j+2*pdist)
      endfor 
    else  #pdist=0
      for j = 1:points
          StY{i,j} = load("-ascii", char( strcat( p, filez{1,i}{j}) ))
          RLQ(j) = StY{i,j}(1)
          DBP(j) = StY{i,j}(2)
          CAOS(j) = StY{i,j}(3)
      endfor
    endif
    
    # run on RLQ only by first element of each pnext sample
    if (xtype == 2)
      figure (i + firstFigure)
      for k = 1:4
        a(k)=1+(k-1)*pnext
        b(k)=a(k)+pnext-1
        usleep(100000)
      endfor
      plot(xs,cell2mat(RLQ(a(1):b(1))),RLQLine,xs,cell2mat(RLQ(a(2):b(2))),
           DBPLine,xs,cell2mat(RLQ(a(3):b(3))),CAOSLine,xs,cell2mat(RLQ(a(4):b(4))),RLQ2Line)
      setLabels4B(titlez(i))
      return  
    endif
        
    
    # parse data acquired for large pdist, e.g. pdist of 16 contains 4 blocks of 4
    if (pnext > 0)
      for k = 1:4
        figure(k + firstFigure)
        a=1+(k-1)*pnext
        b=a+pnext-1
        usleep(100000)
        plot(xs,cell2mat(RLQ(a:b)),RLQLine,xs,cell2mat(DBP(a:b)),DBPLine,xs,cell2mat(CAOS(a:b)),CAOSLine)
        if (xtype == 0)
          setLabels("PER",titlez(k))
        elseif (xtype == 1)
          setLabels("E[H_1]",titlez(k))
        endif
      endfor 
    else # normal plotting of single
      figure (i + firstFigure)
      usleep(100000)
      plot(xs,cell2mat(RLQ),RLQLine,xs,cell2mat(DBP),DBPLine,xs,cell2mat(CAOS),CAOSLine)
      if (xtype == 0)
        setLabels("PER", titlez(i))
      elseif (xtype == 5)
        setLabels("Nodes", titlez(i))
      elseif (xtype == 1)
        setLabels("E[H_1]", titlez(i))
      elseif (xtype == 3)
        setLabelsPConv("PER", titlez(i))
      endif
    endif
  endfor
endfunction

path_sim = home_path_sim
conv_path_sim = home_conv_path_sim
unit_path_sim = home_unit_path_sim

# Static channels experiments #1 (3,4)-firm for Stat and (8,10)-firm for Perf3

StatHe5U={'PerfHe5UPer01.sim','PerfHe5UPer02.sim','PerfHe5UPer03.sim','PerfHe5UPer04.sim','PerfHe5UPer05.sim'}
StatHo5U={'PerfHo5UPer01.sim','PerfHo5UPer02.sim','PerfHo5UPer03.sim','PerfHo5UPer04.sim','PerfHo5UPer05.sim'}
StatHe20U={'PerfHe20UPer01.sim','PerfHe20UPer015.sim','PerfHe20UPer02.sim','PerfHe20UPer025.sim','PerfHe20UPer03.sim'}
StatHo20U={'PerfHo20UPer01.sim','PerfHo20UPer015.sim','PerfHo20UPer02.sim','PerfHo20UPer025.sim','PerfHo20UPer03.sim'}
Stat3He5U={'Perf3He5UPer01.sim','Perf3He5UPer02.sim','Perf3He5UPer03.sim','Perf3He5UPer04.sim','Perf3He5UPer05.sim'}
Stat3Ho5U={'Perf3Ho5UPer01.sim','Perf3Ho5UPer02.sim','Perf3Ho5UPer03.sim','Perf3Ho5UPer04.sim','Perf3Ho5UPer05.sim'}
Stat3He20U={'Perf3He20UPer01.sim','Perf3He20UPer015.sim','Perf3He20UPer02.sim','Perf3He20UPer025.sim','Perf3He20UPer03.sim'}
Stat3Ho20U={'Perf3Ho20UPer01.sim','Perf3Ho20UPer015.sim','Perf3Ho20UPer02.sim','Perf3Ho20UPer025.sim','Perf3Ho20UPer03.sim'}

nFigure=0

Statx=0.1:0.1:0.5
Statfilez={StatHe5U,Stat3He5U,StatHo5U,Stat3Ho5U,StatHe20U,Stat3He20U,StatHo20U,Stat3Ho20U}

stTitle={'5 Users over Heterogeneous Static Channel V_T(PER)',
'5 Users over Homogeneous Static Channel V_T(PER)',
'20 Users over Heterogeneous Static Channel V_T(PER)',
'20 Users over Homogeneous Static Channel V_T(PER)'}

if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 8, numel(Statx), Statx, nFigure, 0, 5, 0)
  nFigure+=numel(Statfilez)
endif


# Markov B=0.5 PER=0.1->0.3 Vary EH1
GEHe10UB05vE={'PerfHeU10Per01MarkovB05VaryEH1.sim', 'PerfHeU10Per015MarkovB05VaryEH1.sim', 'PerfHeU10Per02MarkovB05VaryEH1.sim', 'PerfHeU10Per025MarkovB05VaryEH1.sim', 'PerfHeU10Per03MarkovB05VaryEH1.sim'} 
GEHe20UB05vE={'PerfHeU20Per01MarkovB05VaryEH1.sim', 'PerfHeU20Per015MarkovB05VaryEH1.sim', 'PerfHeU20Per02MarkovB05VaryEH1.sim', 'PerfHeU20Per025MarkovB05VaryEH1.sim', 'PerfHeU20Per03MarkovB05VaryEH1.sim'} 
stTitle={'10 Users under E[H_1]+PER-heterogeneous GE Channel (B=0.5), V_T as function of PER',
'20 Users under E[H_1]+PER-heterogeneous GE Channel (B=0.5), V_T as function of PER'}

Statx=0.1:0.05:0.3
Statfilez={GEHe10UB05vE,GEHe20UB05vE}

if (Plotz >0)
  plotSimes(path_sim, Statfilez, stTitle, 2, numel(Statx), Statx, nFigure, 0, 0, 0)
  nFigure+=numel(Statfilez)
endif


# Markov B=0.7 PER=0.1->0.3 Heterogeneous, func of EH1
EHx=cell2mat({2,8,15,22,29,36,43,50})
blocks=numel(EHx)
heur=4  # RLQ,DBP,CAOS,DBRR
points=heur*blocks

Statfilez={'PerfHeU10Per01MarkovB07.sim','PerfHeU10Per02MarkovB07.sim','PerfHeU10Per03MarkovB07.sim'}
Statx=0.1:0.1:0.3
Eh1_offsets=[0,3,5]
stTitle={'10 Users under Het GE Channel (B=0.7,E[H_1]=5), V_T as function of PER',
'10 Users under Het. GE Channel (B=0.7,E[H_1]=22), V_T as function of PER',
'10 Users under Het. GE Channel (B=0.7,E[H_1]=36), V_T as function of PER'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, numel(Statfilez), numel(Statx), Statx, nFigure, -blocks, 1, Eh1_offsets)
  nFigure+=numel(Statfilez)
endif

Statfilez={'PerfHeU10Per01MarkovB07.sim'}
Statx=EHx
stTitle={'10 Users under Het GE Channel (B=0.7, Avg P=0.1), V_T as function of E[H1]'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, numel(EHx), 
            Statx, nFigure, blocks, 1, 0)
  nFigure+=1
endif

Statfilez={'PerfHeU10Per02MarkovB07.sim'}
Statx=EHx
stTitle={'10 Users under Het GE Channel (B=0.7, Avg P=0.2), V_T as function of E[H1]'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, numel(EHx), 
            Statx, nFigure, blocks, 1, 0)
  nFigure+=1
endif

Statfilez={'PerfHeU10Per03MarkovB07.sim'}
Statx=EHx
stTitle={'10 Users under Het GE Channel (B=0.7, Avg P=0.3), V_T as function of E[H1]'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, numel(EHx), 
            Statx, nFigure, blocks, 1, 0)
  nFigure+=1
endif

# Markov vary B 5U PER=0.2 Heterogeneous, func of EH1

Statfilez={'PerfHoU5Per02Markov.sim'}
stTitle={'5 Users under Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.3',
'5 Users over Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.5',
'5 Users over Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.7',
'5 Users over Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.9'}
Statx=EHx
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, points, 
            Statx, nFigure, points, 1, blocks)
  nFigure+=4
endif
# Diagrams of various B's over E[H1]
Statx=EHx
stTitle={'5 Users over Homogeneous G.E. Scenario, NCQRS V_T as function of E[H_1] for various B'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, numel(Statfilez), points, 
            Statx, nFigure, points, 2, blocks)
  nFigure+=1
endif

# Markov vary B 15U PER=0.2 Heterogeneous, func of EH1
Statfilez={'PerfHoU15Per02Markov.sim'}
Statx=EHx
stTitle={'15 Users under Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.3',
'15 Users under Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.5',
'15 Users under Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.7',
'15 Users under Hom. GE Channel, V_T as function of E[H_1], PER=0.2, B=0.9'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, numel(Statfilez), points, 
            Statx, nFigure, points, 1, blocks)
  nFigure+=4
endif
# Diagrams of various B's over E[H1]
stTitle={'15 Users under Hom. GE Channel, NCQRS V_T as function of E[H_1] for various B'}
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, numel(Statfilez), points, 
            Statx, nFigure, points, 2, blocks)
  nFigure+=1
endif

fullHet1={'PerfHeU15MarkovVaryAllP010.sim','PerfHeU15MarkovVaryAllP015.sim','PerfHeU15MarkovVaryAllP020.sim','PerfHeU15MarkovVaryAllP025.sim','PerfHeU15MarkovVaryAllP030.sim'}
Statfilez={fullHet1}
stTitle={'15 Users Fully Heterogeneous GE Channel, V_T as function of avg.PER'}
Statx=0.1:0.05:0.3
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, numel(Statx), Statx, nFigure, 0, 0, 0)
  nFigure+=1
endif

fullHet1={'Perf2HeU5MarkovVaryAllP010.sim','Perf2HeU5MarkovVaryAllP015.sim','Perf2HeU5MarkovVaryAllP020.sim','Perf2HeU5MarkovVaryAllP025.sim','Perf2HeU5MarkovVaryAllP030.sim'}
Statfilez={fullHet1}
stTitle={'5 Users Fully Heterogeneous GE Channel, V_T as function of avg.PER'}
Statx=0.1:0.05:0.3
if (Plotz > 0)
  plotSimes(path_sim, Statfilez, stTitle, 1, numel(Statx), Statx, nFigure, 0, 0, 0)
  nFigure+=1
endif

# #########################
# # Convergence plots
# #########################

perConvFilez={'ConvHo010UPer01Per.sim', 'ConvHo015UPer01Per.sim'}
Statfilez={perConvFilez}
stTitle={'Convergence for various step sizes from initial PER=0.1'}
Statx=0.2:0.05:0.4
if (Convz > 0)
  plotSimes(conv_path_sim, perConvFilez, stTitle, 1, numel(Statx), Statx, nFigure, 1, 4, "PER step size")
  nFigure+=1
endif

perConvFilez={'ConvHo010UPer02Per.sim', 'ConvHo015UPer02Per.sim'}
Statfilez={perConvFilez}
stTitle={'Convergence for various step sizes from initial PER=0.2'}
Statx=0.2:0.05:0.4
if (Convz == 0)
  plotSimes(conv_path_sim, perConvFilez, stTitle, 1, numel(Statx), Statx, nFigure, 1, 4, "PER step size")
  nFigure+=1
endif

mkConvFilez={'ConvHo10UPer02mk.sim','ConvHo15UPer02mk.sim','ConvHo20UPer02mk.sim'}
Statfilez={mkConvFilez}
stTitle={'Convergence for various (m,k)-firm deadlines','','',''}
Statx=cell2mat({4,6,8,10,12,14,16})
#Statx={'(2,3)', '(3,4)', '(5,6)', '(7,8)', '(9,10)', '(14,15)'}

if (Convz > 0)
  plotSimes(conv_path_sim, mkConvFilez, stTitle, 1, numel(Statx), Statx, nFigure, 0, 4, "k from (m,k)-firm deadlines")
  nFigure+=1
endif

Statx=10:10:100

# Plots of nodes 10,20,...,100 per different PER configurations
StatHoPer01={'LargeNet10UPer01.sim','LargeNet20UPer01.sim','LargeNet30UPer01.sim','LargeNet40UPer01.sim','LargeNet50UPer01.sim','LargeNet60UPer01.sim','LargeNet70UPer01.sim','LargeNet80UPer01.sim','LargeNet90UPer01.sim','LargeNet100UPer01.sim'}
StatHoPer02={'LargeNet10UPer015.sim','LargeNet20UPer015.sim','LargeNet30UPer015.sim','LargeNet40UPer015.sim','LargeNet50UPer015.sim','LargeNet60UPer015.sim','LargeNet70UPer015.sim','LargeNet80UPer015.sim','LargeNet90UPer015.sim','LargeNet100UPer015.sim'}
StatHoPer03={'LargeNet10UPer02.sim','LargeNet20UPer02.sim','LargeNet30UPer02.sim','LargeNet40UPer02.sim','LargeNet50UPer02.sim','LargeNet60UPer02.sim','LargeNet70UPer02.sim','LargeNet80UPer02.sim','LargeNet90UPer02.sim','LargeNet100UPer02.sim'}
StatHoPer04={'LargeNet10UPer025.sim','LargeNet20UPer025.sim','LargeNet30UPer025.sim','LargeNet40UPer025.sim','LargeNet50UPer025.sim','LargeNet60UPer025.sim','LargeNet70UPer025.sim','LargeNet80UPer025.sim','LargeNet90UPer025.sim','LargeNet100UPer025.sim'}

stTitle={'V_T for users under Hom. Static Channel, PER=0.1',
'V_T for users under Hom. Static Channel, PER=0.15',
'V_T for users under Hom. Static Channel, PER=0.2',
'V_T for users under Hom. Static Channel, PER=0.25'}

Statfilez={StatHoPer01,StatHoPer02,StatHoPer03,StatHoPer04}
if (Unitz > 0)
  plotSimes(unit_path_sim, Statfilez, stTitle, 4, numel(Statx), Statx, nFigure, 0, 5, 0)
  nFigure+=numel(Statfilez)
endif

# Scenario G1E with 10,..,100 units but 

Statfilez={StatHoPer01,StatHoPer02,StatHoPer03,StatHoPer04}
stTitle={'V_T for users under GE Channel B=0.4 E[h1]=5, PER=0.1',
'V_T for users under GE Channel B=0.4 E[h1]=5, PER=0.15',
'V_T for users under GE Channel B=0.4 E[h1]=5, PER=0.2',
'V_T for users under GE Channel B=0.4 E[h1]=5, PER=0.25'}

# Plots of nodes 10,20,...,100 per different PER configurations
StatG1EPer01={'LargeNetG1E10UPer01.sim','LargeNetG1E20UPer01.sim','LargeNetG1E30UPer01.sim','LargeNetG1E40UPer01.sim','LargeNetG1E50UPer01.sim','LargeNetG1E60UPer01.sim','LargeNetG1E70UPer01.sim','LargeNetG1E80UPer01.sim','LargeNetG1E90UPer01.sim','LargeNetG1E100UPer01.sim'}
StatG1EPer02={'LargeNetG1E10UPer015.sim','LargeNetG1E20UPer015.sim','LargeNetG1E30UPer015.sim','LargeNetG1E40UPer015.sim','LargeNetG1E50UPer015.sim','LargeNetG1E60UPer015.sim','LargeNetG1E70UPer015.sim','LargeNetG1E80UPer015.sim','LargeNetG1E90UPer015.sim','LargeNetG1E100UPer015.sim'}
StatG1EPer03={'LargeNetG1E10UPer02.sim','LargeNetG1E20UPer02.sim','LargeNetG1E30UPer02.sim','LargeNetG1E40UPer02.sim','LargeNetG1E50UPer02.sim','LargeNetG1E60UPer02.sim','LargeNetG1E70UPer02.sim','LargeNetG1E80UPer02.sim','LargeNetG1E90UPer02.sim','LargeNetG1E100UPer02.sim'}
StatG1EPer04={'LargeNetG1E10UPer025.sim','LargeNetG1E20UPer025.sim','LargeNetG1E30UPer025.sim','LargeNetG1E40UPer025.sim','LargeNetG1E50UPer025.sim','LargeNetG1E60UPer025.sim','LargeNetG1E70UPer025.sim','LargeNetG1E80UPer025.sim','LargeNetG1E90UPer025.sim','LargeNetG1E100UPer025.sim'}
Statfilez={StatG1EPer01,StatG1EPer02,StatG1EPer03,StatG1EPer04}
if (Unitz > 0)
  plotSimes(unit_path_sim, Statfilez, stTitle, 4, numel(Statx), Statx, nFigure, 0, 5, 0)
  nFigure+=numel(Statfilez)
endif

# Scenario G2E with 10,..,100 units but 

stTitle={'V_T for users under GE Channel B=0.4 E[h1]=250, PER=0.1',
'V_T for users under GE Channel B=0.4 E[h1]=250, PER=0.15',
'V_T for users under GE Channel B=0.4 E[h1]=250, PER=0.2',
'V_T for users under GE Channel B=0.4 E[h1]=250, PER=0.25'}

# Plots of nodes 10,20,...,100 per different PER configurations
StatG2EPer01={'LargeNetG2E10UPer01.sim','LargeNetG2E20UPer01.sim','LargeNetG2E30UPer01.sim','LargeNetG2E40UPer01.sim','LargeNetG2E50UPer01.sim','LargeNetG2E60UPer01.sim','LargeNetG2E70UPer01.sim','LargeNetG2E80UPer01.sim','LargeNetG2E90UPer01.sim','LargeNetG2E100UPer01.sim'}
StatG2EPer02={'LargeNetG2E10UPer015.sim','LargeNetG2E20UPer015.sim','LargeNetG2E30UPer015.sim','LargeNetG2E40UPer015.sim','LargeNetG2E50UPer015.sim','LargeNetG2E60UPer015.sim','LargeNetG2E70UPer015.sim','LargeNetG2E80UPer015.sim','LargeNetG2E90UPer015.sim','LargeNetG2E100UPer015.sim'}
StatG2EPer03={'LargeNetG2E10UPer02.sim','LargeNetG2E20UPer02.sim','LargeNetG2E30UPer02.sim','LargeNetG2E40UPer02.sim','LargeNetG2E50UPer02.sim','LargeNetG2E60UPer02.sim','LargeNetG2E70UPer02.sim','LargeNetG2E80UPer02.sim','LargeNetG2E90UPer02.sim','LargeNetG2E100UPer02.sim'}
StatG2EPer04={'LargeNetG2E10UPer025.sim','LargeNetG2E20UPer025.sim','LargeNetG2E30UPer025.sim','LargeNetG2E40UPer025.sim','LargeNetG2E50UPer025.sim','LargeNetG2E60UPer025.sim','LargeNetG2E70UPer025.sim','LargeNetG2E80UPer025.sim','LargeNetG2E90UPer025.sim','LargeNetG2E100UPer025.sim'}
Statfilez={StatG2EPer01,StatG2EPer02,StatG2EPer03,StatG2EPer04}
if (Unitz > 0)
  plotSimes(unit_path_sim, Statfilez, stTitle, 4, numel(Statx), Statx, nFigure, 0, 5, 0)
  nFigure+=numel(Statfilez)
endif

#######################################################################
# Optimizations over parameter space, experiments like alphagamma
#######################################################################

function plot_surf(file,x_s,x_e,x_num,y_s,y_e,y_num,x_lbl,y_lbl,z_lbl,tit)
  alpha={}
  x_plot = linspace(x_s,x_e,x_num);
  y_plot = linspace(y_s,y_e,y_num);
  [XI,YI] = meshgrid(x_plot,y_plot);
  z_plot = load("-ascii", char( file ))
  ZI=reshape(z_plot(:),y_num,x_num);
  figure()
  mesh(XI,YI,ZI);
  xlabel(x_lbl)
  ylabel(y_lbl, "rotation", 0)
  #zlabel(z_lbl)
  grid("on")
  #title(tit)
endfunction

function plot_simple(file,x_s,x_e,x_num,x_lbl,y_lbl,tit)
  figure()
  x = linspace(x_s,x_e,x_num);
  y = load("-ascii", char( file ))
  plot(x,y);
  xlabel(x_lbl)
  ylabel(y_lbl, "rotation", 0)
  grid("on")
  title(tit)
endfunction

path='C:\Users\Matusovsky\Desktop\Masters-UC\results\experiments\FindAlphaGamma\04_Feb_2016\'
f1='FindOptAlphaGamma1.sim' #alpha-gamma static hom
f2='FindOptAlphaGamma2.sim' #rew static hom
#f3='FindOptAlphaGamma3.sim' #vt(r_n)
f4='FindOptAlphaGamma4.sim' #alpha-gamma static het
f5='FindOptAlphaGamma5.sim' #rew static het
f6='FindOptAlphaGamma6.sim' #ge hom
f7='FindOptAlphaGamma7.sim' #ge het
f8='FindOptAlphaGamma8.sim' #sarsa-q compare
f9='FindOptAlphaGamma9.sim' # reward takes from 7+6
f10='FindOptAlphaGamma10.sim' # reward takes from 7+6

# Experiment with number of slots requested by a radio depends on the alpha/gamma
if (opt > 0)
  # for homogeneous scenario
  tit='Violation rate as function of discount and learning factors'
  plot_surf(strcat(path,f1),0,0.95,20,0.0000005,0.0003,60,"\\gamma","\\alpha",'V_T',tit)
  usleep(100000)
  tit='Violation rate as function of slot and stream violation rewards @r_d=-10'
  plot_surf(strcat(path,f2),-0.01,-1,50,-30,-120,19,"r_n","r_v",'V_T',tit)
  usleep(100000)
  #tit='Violation rate as function of slot regrets @r_d=-10,r_v=-75'
  #plot_simple(strcat(path,f3),-0.2,0,201,"r_n",'V_T',tit)
  #usleep(100000)
  # for heterogeneous scenario
  tit='Violation rate as function of discount and learning factors'
  plot_surf(strcat(path,f4),0,0.95,20,0.0000005,0.0003,60,"\\gamma","\\alpha",'V_T',tit)
  usleep(100000)
  tit='Violation rate as function of slot and stream violation regrets @r_d=-10'
  plot_surf(strcat(path,f5),-0.01,-1,50,-120,-30,19,"r_n","r_v",'V_T',tit)
  usleep(100000)
  # for markov channel b=0.5 eh1=50 per=0.2 hom alpha-gamma
  tit='Violation rate as function of discount and learning factors'
  plot_surf(strcat(path,f6),0,0.95,20,0.0000005,0.0003,30,"\\gamma","\\alpha",'V_T',tit)
  usleep(100000)
  # for markov channel b=0.5 eh1=50 per=0.2 het alpha-gamma
  tit='Violation rate as function of discount and learning factors'
  plot_surf(strcat(path,f7),0,0.95,20,0.0000005,0.0003,30,"\\gamma","\\alpha",'V_T',tit)
  usleep(100000)
  # for markov channel b=0.5 eh1=50 per=0.2 het rewards
  tit='Violation rate as function of slot and stream violation regrets @r_d=-10'
  plot_surf(strcat(path,f9),-0.01,-1,50,-120,-30,19,"r_n","r_v",'V_T',tit)
  usleep(100000)
  # for markov channel b=0.5 eh1=50 per=0.2 hom rewards
  tit='Violation rate as function of slot and stream violation regrets @r_d=-10'
  plot_surf(strcat(path,f10),-0.01,-1,50,-120,-30,19,"r_n","r_v",'V_T',tit)
  usleep(100000)
  # for homogeneous scenario SARSA alpha-gamma search
  tit='Violation rate as function of discount and learning factors'
  plot_surf(strcat(path,f8),0,0.95,20,0.0000005,0.0003,30,"\\gamma","\\alpha",'V_T',tit)
  usleep(100000)

  
endif

/*
 * Copyright 2022 Barabas Raffai
 *
 * This file is part of ShrapnelDSP.
 *
 * ShrapnelDSP is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * ShrapnelDSP is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ShrapnelDSP. If not, see <https://www.gnu.org/licenses/>.
 */

static float fir_coeff[] = {
    -0.0037564742378890514,
    -0.0194765105843544,
    -0.08748021721839905,
    -0.26966428756713867,
    -0.5808219313621521,
    -0.9063171744346619,
    -1.0386521816253662,
    -0.8603169322013855,
    -0.46721968054771423,
    -0.04791257157921791,
    0.2867630124092102,
    0.4984447658061981,
    0.574799656867981,
    0.5154806971549988,
    0.3306923508644104,
    0.060714464634656906,
    -0.20888927578926086,
    -0.37036028504371643,
    -0.3820297122001648,
    -0.28964871168136597,
    -0.14016802608966827,
    0.04671409726142883,
    0.2305663675069809,
    0.33528295159339905,
    0.30148687958717346,
    0.15575389564037323,
    -0.007246615830808878,
    -0.1165938749909401,
    -0.15145261585712433,
    -0.12392398715019226,
    -0.05786096304655075,
    0.03507285565137863,
    0.13526953756809235,
    0.21170289814472198,
    0.2343493103981018,
    0.1867852360010147,
    0.09546754509210587,
    0.009756524115800858,
    -0.03376837819814682,
    -0.02573435753583908,
    0.009972216561436653,
    0.04652196541428566,
    0.07472579181194305,
    0.0879240334033966,
    0.0800890326499939,
    0.051414161920547485,
    0.01515804510563612,
    -0.0007666537421755493,
    0.012387222610414028,
    0.03294295072555542,
    0.047237593680620193,
    0.06432503461837769,
    0.09276732057332993,
    0.11616240441799164,
    0.11090561747550964,
    0.08753321319818497,
    0.07502544671297073,
    0.0789351612329483,
    0.07730638235807419,
    0.050708722323179245,
    0.01286821998655796,
    -0.008763324469327927,
    -0.005359808448702097,
    0.013226817362010479,
    0.02767263725399971,
    0.029534989967942238,
    0.028516555204987526,
    0.03406006097793579,
    0.04741372913122177,
    0.0678158849477768,
    0.09569378942251205,
    0.12033625692129135,
    0.12075372040271759,
    0.09422051906585693,
    0.05760864540934563,
    0.02334357425570488,
    -0.005383335053920746,
    -0.02656237781047821,
    -0.03096957691013813,
    -0.009539122693240643,
    0.029128750786185265,
    0.06635220348834991,
    0.0912904366850853,
    0.10650883615016937,
    0.1187136247754097,
    0.12121295183897018,
    0.10162882506847382,
    0.06396611034870148,
    0.025461357086896896,
    -0.0013506790855899453,
    -0.016417788341641426,
    -0.021823152899742126,
    -0.010216502472758293,
    0.02146880328655243,
    0.058606214821338654,
    0.08508080244064331,
    0.09790182113647461,
    0.10373619198799133,
    0.11126091331243515,
    0.12373356521129608,
    0.1369648426771164,
    0.142042338848114,
    0.13163070380687714,
    0.10776324570178986,
    0.08002027124166489,
    0.057062286883592606,
    0.0422426238656044,
    0.03383830934762955,
    0.028147650882601738,
    0.022562524303793907,
    0.01655130460858345,
    0.011252408847212791,
    0.00872215535491705,
    0.01241507101804018,
    0.02441943995654583,
    0.04053754732012749,
    0.052236028015613556,
    0.054180413484573364,
    0.04924267902970314,
    0.0445871539413929,
    0.043778084218502045,
    0.044408395886421204,
    0.041528183966875076,
    0.03408752754330635,
    0.02638247422873974,
    0.02095939964056015,
    0.015301629900932312,
    0.008153784088790417,
    0.0040175351314246655,
    0.008534149266779423,
    0.02159642055630684,
    0.03656883165240288,
    0.04687534645199776,
    0.05171499401330948,
    0.05292370542883873,
    0.05002389848232269,
    0.04179229959845543,
    0.029276829212903976,
    0.015537804923951626,
    0.003570778062567115,
    -0.0047484710812568665,
    -0.00816650502383709,
    -0.007272896356880665,
    -0.004781515337526798,
    -0.0023701495956629515,
    4.1200113628292456e-05,
    0.0014204599428921938,
    -0.0007130166050046682,
    -0.007389390375465155,
    -0.01816841959953308,
    -0.03251173347234726,
    -0.04821757599711418,
    -0.06093612685799599,
    -0.06629174202680588,
    -0.061788614839315414,
    -0.04751215875148773,
    -0.026231583207845688,
    -0.0027790414169430733,
    0.018112724646925926,
    0.033463168889284134,
    0.04180353879928589,
    0.04280052334070206,
    0.03747948259115219,
    0.027628561481833458,
    0.014538433402776718,
    -0.0008242146577686071,
    -0.016671188175678253,
    -0.029882842674851418,
    -0.03644527867436409,
    -0.03278292343020439,
    -0.017462430521845818,
    0.007051154039800167,
    0.03397012874484062,
    0.05528358370065689,
    0.06641294062137604,
    0.06760620325803757,
    0.0612633042037487,
    0.049977414309978485,
    0.03765544295310974,
    0.02876364439725876,
    0.024916483089327812,
    0.024167725816369057,
    0.023442506790161133,
    0.020884664729237556,
    0.015640761703252792,
    0.006671412847936153,
    -0.005913218483328819,
    -0.01907447539269924,
    -0.028054753318428993,
    -0.02989596128463745,
    -0.025532154366374016,
    -0.018342338502407074,
    -0.01233007200062275,
    -0.011565517634153366,
    -0.018694384023547173,
    -0.033133238554000854,
    -0.05103873834013939,
    -0.06711363792419434,
    -0.0772828608751297,
    -0.08035674691200256,
    -0.07714332640171051,
    -0.0687483623623848,
    -0.056886106729507446,
    -0.04503145441412926,
    -0.037856101989746094,
    -0.03853832185268402,
    -0.04630838707089424,
    -0.05750533193349838,
    -0.06837014853954315,
    -0.07641368359327316,
    -0.08012834936380386,
    -0.0781327337026596,
    -0.0701565220952034,
    -0.058463890105485916,
    -0.04642091318964958,
    -0.03612952679395676,
    -0.027894960716366768,
    -0.021285640075802803,
    -0.016199957579374313,
    -0.012791545130312443,
    -0.011271164752542973,
    -0.012176073156297207,
    -0.016162855550646782,
    -0.023242389783263206,
    -0.03219522163271904,
    -0.04103313013911247,
    -0.047839898616075516,
    -0.05131106451153755,
    -0.05111429840326309,
    -0.04757564514875412,
    -0.04122688248753548,
    -0.0329035259783268,
    -0.02438868023455143,
    -0.018043989315629005,
    -0.015078673139214516,
    -0.015089812688529491,
    -0.016884872689843178,
    -0.019541116431355476,
    -0.02298358827829361,
    -0.027195405215024948,
    -0.03151925653219223,
    -0.03472668677568436,
    -0.03570104390382767,
    -0.034775953739881516,
    -0.033544398844242096,
    -0.03309851512312889,
    -0.03345469385385513,
    -0.033738624304533005,
    -0.03235357627272606,
    -0.02809050865471363,
    -0.02160836197435856,
    -0.0156696904450655,
    -0.013488708063960075,
    -0.016786543652415276,
    -0.024701224640011787,
    -0.03375251591205597,
    -0.03980088233947754,
    -0.04063611850142479,
    -0.03669653832912445,
    -0.029845058917999268,
    -0.022224046289920807,
    -0.016459941864013672,
    -0.015229298733174801,
    -0.019251910969614983,
    -0.026527656242251396,
    -0.033926378935575485,
    -0.03884386271238327,
    -0.0398489385843277,
    -0.03745930641889572,
    -0.03426773101091385,
    -0.03319589048624039,
    -0.03587108850479126,
    -0.04262905567884445,
    -0.05290330573916435,
    -0.0648411214351654,
    -0.07552596181631088,
    -0.0826861783862114,
    -0.08568792790174484,
    -0.08497437834739685,
    -0.08131538331508636,
    -0.07588314265012741,
    -0.07054484635591507,
    -0.06691592931747437,
    -0.06498613208532333,
    -0.0632193312048912,
    -0.06005058065056801,
    -0.05523053929209709,
    -0.04990329220890999,
    -0.04563998803496361,
    -0.0436055064201355,
    -0.04416409134864807,
    -0.046566110104322433,
    -0.04921175166964531,
    -0.050633423030376434,
    -0.050132445991039276,
    -0.047294292598962784,
    -0.04165000468492508,
    -0.03333599492907524,
    -0.023698054254055023,
    -0.015099850483238697,
    -0.009767184033989906,
    -0.008634660392999649,
    -0.011079149320721626,
    -0.015326639637351036,
    -0.019305400550365448,
    -0.02120974287390709,
    -0.019891390576958656,
    -0.015641702339053154,
    -0.010339420288801193,
    -0.0065151541493833065,
    -0.005972495302557945,
    -0.008714621886610985,
    -0.013232363387942314,
    -0.01758558116853237,
    -0.020117877051234245,
    -0.019905786961317062,
    -0.017052100971341133,
    -0.012886131182312965,
    -0.009465807117521763,
    -0.0081764105707407,
    -0.009166651405394077,
    -0.012197955511510372,
    -0.01720809005200863,
    -0.02399374172091484,
    -0.031756691634655,
    -0.038934845477342606,
    -0.04381569102406502,
    -0.0455235056579113,
    -0.04421360418200493,
    -0.040421586483716965,
    -0.03471613675355911,
    -0.028103388845920563,
    -0.02191978506743908,
    -0.01712588593363762,
    -0.014133544638752937,
    -0.012922202236950397,
    -0.013102054595947266,
    -0.014132590964436531,
    -0.015427269972860813,
    -0.016258763149380684,
    -0.0157249104231596,
    -0.013103004544973373,
    -0.008443421684205532,
    -0.002616799669340253,
    0.0029208620544523,
    0.006572186481207609,
    0.0075727407820522785,
    0.006378610618412495,
    0.004062459338456392,
    0.0014312975108623505,
    -0.0014373742742463946,
    -0.004770706873387098,
    -0.00840942095965147,
    -0.01186327449977398,
    -0.014839318580925465,
    -0.017229333519935608,
    -0.018865885213017464,
    -0.019623536616563797,
    -0.01959698647260666,
    -0.019037455320358276,
    -0.0180219653993845,
    -0.01659233495593071,
    -0.015209903940558434,
    -0.014478296041488647,
    -0.014579030685126781,
    -0.01538778841495514,
    -0.016952892765402794,
    -0.019235428422689438,
    -0.021462000906467438,
    -0.0224313884973526,
    -0.02134745754301548,
    -0.018301837146282196,
    -0.01436549797654152,
    -0.010883968323469162,
    -0.008562388829886913,
    -0.007409405428916216,
    -0.00694100558757782,
    -0.0063561126589775085,
    -0.005018407944589853,
    -0.003014869522303343,
    -0.0010012337006628513,
    0.000773956417106092,
    0.0025296686217188835,
    0.004399014171212912,
    0.006410346832126379,
    0.00835527665913105,
    0.010022538714110851,
    0.01149713434278965,
    0.012951045297086239,
    0.014542588032782078,
    0.01610627770423889,
    0.01710520125925541,
    0.017360566183924675,
    0.017343085259199142,
    0.0176528487354517,
    0.01859612949192524,
    0.020369242876768112,
    0.02322482317686081,
    0.026860492303967476,
    0.030123921111226082,
    0.031756237149238586,
    0.0312105230987072,
    0.029044339433312416,
    0.026604466140270233,
    0.025136975571513176,
    0.02518281526863575,
    0.026627089828252792,
    0.028800738975405693,
    0.03070429153740406,
    0.0315006822347641,
    0.030407635495066643,
    0.026784326881170273,
    0.02095768041908741,
    0.014511692337691784,
    0.009566562250256538,
    0.007508228067308664,
    0.008213026449084282,
    0.010539781302213669,
    0.013413071632385254,
    0.016163844615221024,
    0.018063096329569817,
    0.01837853342294693,
    0.017062554135918617,
    0.01463649794459343,
    0.01174165215343237,
    0.009182150475680828,
    0.007584500592201948,
    0.0071178157813847065,
    0.007631204556673765,
    0.00883105956017971,
    0.010448778979480267,
    0.01199730858206749,
    0.01275942474603653,
    0.012454099021852016,
    0.011492142453789711,
    0.010311220772564411,
    0.009009230881929398,
    0.007863461039960384,
    0.007410241756588221,
    0.00788551103323698,
    0.009153944440186024,
    0.010924229398369789,
    0.012879380024969578,
    0.01476728543639183,
    0.016222326084971428,
    0.016954435035586357,
    0.01703772507607937,
    0.016646480187773705,
    0.016038015484809875,
    0.01585807278752327,
    0.01700383424758911,
    0.019863730296492577,
    0.02404233068227768,
    0.02882981300354004,
    0.03348923847079277,
    0.03760652244091034,
    0.04125063866376877,
    0.0444934219121933,
    0.046975672245025635,
    0.04798082262277603,
    0.047155462205410004,
    0.04519657790660858,
    0.04336443170905113,
    0.04248249530792236,
    0.042632777243852615,
    0.043422017246484756,
    0.04431718960404396,
    0.044846534729003906,
    0.04483090713620186,
    0.04434332996606827,
    0.043275926262140274,
    0.041288312524557114,
    0.03809576854109764,
    0.03363563120365143,
    0.028259456157684326,
    0.022917192429304123,
    0.018733112141489983,
    0.016111934557557106,
    0.014731538482010365,
    0.014390407130122185,
    0.015210277400910854,
    0.017190612852573395,
    0.01996741257607937,
    0.022862199693918228,
    0.02501807175576687,
    0.025630956515669823,
    0.0244301650673151,
    0.02192607894539833,
    0.018997251987457275,
    0.0164243932813406,
    0.01484654936939478,
    0.014682198874652386,
    0.015685833990573883,
    0.01675872504711151,
    0.016644224524497986,
    0.0151270916685462,
    0.013264201581478119,
    0.012194594368338585,
    0.012169956229627132,
#if 0
    0.012667597271502018,
    0.013080847449600697,
    0.013511157594621181,
    0.014826846309006214,
    0.01767025515437126,
    0.02161911129951477,
    0.025674525648355484,
    0.02929791994392872,
    0.03230809420347214,
    0.03425689414143562,
    0.03472146391868591,
    0.03386275842785835,
    0.032391343265771866,
    0.031098617240786552,
    0.03043729066848755,
    0.030431952327489853,
    0.030991330742836,
    0.032094310969114304,
    0.033448733389377594,
    0.03444906696677208,
    0.03454728052020073,
    0.033542271703481674,
    0.0315813384950161,
    0.029008975252509117,
    0.026468845084309578,
    0.024649785831570625,
    0.023706888779997826,
    0.023331575095653534,
    0.023256445303559303,
    0.023419570177793503,
    0.023797204717993736,
    0.024403253570199013,
    0.025263171643018723,
    0.02611466869711876,
    0.026380030438303947,
    0.025504160672426224,
    0.02333340048789978,
    0.020391441881656647,
    0.017669660970568657,
    0.016035664826631546,
    0.01570277474820614,
    0.01613239385187626,
    0.016571395099163055,
    0.016670577228069305,
    0.016758084297180176,
    0.017385423183441162,
    0.018593063578009605,
    0.020078402012586594,
    0.021517205983400345,
    0.02248048037290573,
    0.02271643839776516,
    0.02256864868104458,
    0.022921690717339516,
    0.02433740720152855,
    0.026200197637081146,
    0.027183178812265396,
    0.02626717835664749,
    0.02330530621111393,
    0.019096268340945244,
    0.014925085939466953,
    0.011885189451277256,
    0.010440882295370102,
    0.010513742454349995,
    0.011720103211700916,
    0.013482470065355301,
    0.015207181684672832,
    0.016429832205176353,
    0.016900554299354553,
    0.016552507877349854,
    0.015370359644293785,
    0.013281464576721191,
    0.010164372622966766,
    0.0061484286561608315,
    0.0019366645719856024,
    -0.001466479618102312,
    -0.0033673469442874193,
    -0.003552459180355072,
    -0.0021583971101790667,
    0.0003475575940683484,
    0.003198848804458976,
    0.0056252614594995975,
    0.007060409523546696,
    0.007120456080883741,
    0.0057320063933730125,
    0.0033034817315638065,
    0.0007996452623046935,
    -0.0007488769479095936,
    -0.0009876807453110814,
    -0.00017427258717361838,
    0.0012603977229446173,
    0.002872299635782838,
    0.004260183311998844,
    0.005191862117499113,
    0.005746032111346722,
    0.0063775572925806046,
    0.00768424104899168,
    0.00982810091227293,
    0.01220938190817833,
    0.013893883675336838,
    0.014374190010130405,
    0.013879808597266674,
    0.012989381328225136,
    0.012083880603313446,
    0.01137850247323513,
    0.011134369298815727,
    0.011430439539253712,
    0.011851547285914421,
    0.011761197820305824,
    0.010824644938111305,
    0.008928338065743446,
    0.006227536592632532,
    0.0034251962788403034,
    0.0014928849413990974,
    0.0011095782974734902,
    0.0021941643208265305,
    0.0040814257226884365,
    0.0060868896543979645,
    0.007676845882087946,
    0.008473244495689869,
    0.008278281427919865,
    0.006995668634772301,
    0.00461440859362483,
    0.0013210115721449256,
    -0.0022626854479312897,
    -0.0050735799595713615,
    -0.0061197723262012005,
    -0.004985771607607603,
    -0.002202505711466074,
    0.0009253703756257892,
    0.0030096927657723427,
    0.003092207247391343,
    0.0011544079752638936,
    -0.0018490190850570798,
    -0.004573100246489048,
    -0.006089937407523394,
    -0.006246119271963835,
    -0.005072025116533041,
    -0.002710374305024743,
    -6.013158417772502e-05,
    0.001283355406485498,
    0.00011211908713448793,
    -0.0032640458084642887,
    -0.007320340722799301,
    -0.010680928826332092,
    -0.01276430580765009,
    -0.013370398432016373,
    -0.012386827729642391,
    -0.010405776090919971,
    -0.009143153205513954,
    -0.01030009612441063,
    -0.01383979618549347,
    -0.01796651817858219,
    -0.021184029057621956,
    -0.023280393332242966,
    -0.024275321513414383,
    -0.023882493376731873,
    -0.021934494376182556,
    -0.019314590841531754,
    -0.017555493861436844,
    -0.017288193106651306,
    -0.018431395292282104,
    -0.020510723814368248,
    -0.022615764290094376,
    -0.02374863065779209,
    -0.02282482385635376,
    -0.019471194595098495,
    -0.014772249385714531,
    -0.010701031424105167,
    -0.008412222377955914,
    -0.007093144580721855,
    -0.005827171262353659,
    -0.005256466567516327,
    -0.005895149894058704,
    -0.006580915302038193,
    -0.005866005551069975,
    -0.00393851101398468,
    -0.00217211851850152,
    -0.001403873786330223,
    -0.0015668115811422467,
    -0.002807294251397252,
    -0.005532883573323488,
    -0.008818492293357849,
    -0.010523042641580105,
    -0.009665886871516705,
    -0.007290651090443134,
    -0.004579862114042044,
    -0.0017966395244002342,
    0.0006327166920527816,
    0.0016954891616478562,
    0.00021967216162011027,
    -0.004262316040694714,
    -0.010594682767987251,
    -0.016695521771907806,
    -0.020722122862935066,
    -0.021453069522976875,
    -0.018414292484521866,
    -0.012581798247992992,
    -0.006965294014662504,
    -0.005015093367546797,
    -0.007639625109732151,
    -0.012222744524478912,
    -0.015374151058495045,
    -0.015825778245925903,
    -0.014330542646348476,
    -0.012108058668673038,
    -0.009963639080524445,
    -0.00876807514578104,
    -0.009222655557096004,
    -0.010892442427575588,
    -0.012706685811281204,
    -0.014398621395230293,
    -0.016686325892806053,
    -0.019466014578938484,
    -0.021540645509958267,
    -0.02215111069381237,
    -0.02106013335287571,
    -0.018314369022846222,
    -0.014688020572066307,
    -0.011694982647895813,
    -0.010792529210448265,
    -0.012482999823987484,
    -0.01590399630367756,
    -0.01939273811876774,
    -0.022027572616934776,
    -0.024065371602773666,
    -0.025735562667250633,
    -0.026393473148345947,
    -0.025452524423599243,
    -0.023611528798937798,
    -0.02207261323928833,
    -0.021382808685302734,
    -0.021580800414085388,
    -0.022536905482411385,
    -0.02387155592441559,
    -0.02467862144112587,
    -0.023938443511724472,
    -0.02188674733042717,
    -0.020200831815600395,
    -0.020232779905200005,
    -0.02181059867143631,
    -0.023759840056300163,
    -0.024907618761062622,
    -0.02440781518816948,
    -0.021947938948869705,
    -0.018380828201770782,
    -0.015306007117033005,
    -0.013914866372942924,
    -0.014377020299434662,
    -0.015831993892788887,
    -0.017094317823648453,
    -0.017275094985961914,
    -0.015949109569191933,
    -0.013060144148766994,
    -0.00891135260462761,
    -0.004393883049488068,
    -0.0006655515753664076,
    0.0013842076295986772,
    0.0013522113440558314,
    -0.0005375618347898126,
    -0.0034089458640664816,
    -0.006268456112593412,
    -0.008552984334528446,
    -0.00980344694107771,
    -0.009788923896849155,
    -0.009047492407262325,
    -0.008437230251729488,
    -0.008501425385475159,
    -0.009378382004797459,
    -0.010874241590499878,
    -0.01246032677590847,
    -0.013575769029557705,
    -0.013973276130855083,
    -0.013638031668961048,
    -0.01283829566091299,
    -0.012193923816084862,
    -0.012252753600478172,
    -0.013018688187003136,
    -0.014213775284588337,
    -0.015633411705493927,
    -0.016974862664937973,
    -0.01782640814781189,
    -0.017904605716466904,
    -0.01737983711063862,
    -0.01672845333814621,
    -0.01641753315925598,
    -0.01682618446648121,
    -0.0175645649433136,
    -0.017464229837059975,
    -0.015736116096377373,
    -0.012709013186395168,
    -0.009568109177052975,
    -0.007487077731639147,
    -0.00705380504950881,
    -0.008189843967556953,
    -0.010601675137877464,
    -0.014033481478691101,
    -0.01768559217453003,
    -0.020289376378059387,
    -0.0210472010076046,
    -0.019982600584626198,
    -0.017554471269249916,
    -0.014379601925611496,
    -0.011261471547186375,
    -0.00914171151816845,
    -0.008754960261285305,
    -0.010111899115145206,
    -0.012401849031448364,
    -0.014594377018511295,
    -0.01595299504697323,
    -0.0160529762506485,
    -0.015047223307192326,
    -0.013760401867330074,
    -0.013061003759503365,
    -0.013364626094698906,
    -0.014479964040219784,
    -0.015767093747854233,
    -0.016509750857949257,
    -0.0162662360817194,
    -0.015117899514734745,
    -0.013354857452213764,
    -0.011083513498306274,
    -0.008481999859213829,
    -0.006120575591921806,
    -0.004678015131503344,
    -0.004389486741274595,
    -0.004998540971428156,
    -0.0060854326002299786,
    -0.007075992878526449,
    -0.007325080689042807,
    -0.006630670744925737,
    -0.005232386756688356,
    -0.003346713026985526,
    -0.0011623294558376074,
    0.0009810286574065685,
    0.0026913625188171864,
    0.0037651825696229935,
    0.004224757198244333,
    0.004144548438489437,
    0.0036510322242975235,
    0.0029226571787148714,
    0.0021723415702581406,
    0.0017300753388553858,
    0.0018389858305454254,
    0.002596363192424178,
    0.0040281107649207115,
    0.00578618748113513,
    0.0072520580142736435,
    0.008035651408135891,
    0.008110797964036465,
    0.00762157654389739,
    0.006653149146586657,
    0.005302912555634975,
    0.0038032885640859604,
    0.002427150262519717,
    0.001419589389115572,
    0.0009085062774829566,
    0.0008148422930389643,
    0.000894963915925473,
    0.0008294574217870831,
    0.000373419898096472,
    -0.000497916538733989,
    -0.001567653613165021,
    -0.002576849889010191,
    -0.00350857968442142,
    -0.004390930291265249,
    -0.004981392994523048,
    -0.005074962042272091,
    -0.004878247156739235,
    -0.004830504301935434,
    -0.005123015493154526,
    -0.005574045702815056,
    -0.005946486722677946,
    -0.006016282830387354,
    -0.005530158057808876,
    -0.004487397614866495,
    -0.0033083679154515266,
    -0.002619423670694232,
    -0.0027983454056084156,
    -0.003668748075142503,
    -0.004681708756834269,
    -0.0053249541670084,
    -0.005430778954178095,
    -0.005145102273672819,
    -0.004561575595289469,
    -0.0036429802421480417,
    -0.002597747603431344,
    -0.0019088124390691519,
    -0.0018444585148245096,
    -0.002151607535779476,
    -0.002326628193259239,
    -0.002262240508571267,
    -0.002301550703123212,
    -0.002525929594412446,
    -0.00244458788074553,
    -0.0015323222614824772,
    0.00018250272842124104,
    0.0023949157912284136,
    0.00490416307002306,
    0.007288073655217886,
    0.008847308345139027,
    0.009156627580523491,
    0.008537606336176395,
    0.00788150168955326,
    0.007931357249617577,
    0.00876956433057785,
    0.010000002570450306,
    0.011405225843191147,
    0.013017166405916214,
    0.014543598517775536,
    0.015331939794123173,
    0.014919954352080822,
    0.01348300650715828,
    0.011800543405115604,
    0.010721712373197079,
    0.010736430063843727,
    0.011908913031220436,
    0.013913276605308056,
    0.01611192896962166,
    0.017671683803200722,
    0.01801079884171486,
    0.017157025635242462,
    0.01549304649233818,
    0.013578996062278748,
    0.012063436210155487,
    0.011433969251811504,
    0.011852589435875416,
    0.013037567026913166,
    0.014314013533294201,
    0.014946342445909977,
    0.014530148357152939,
    0.013066131621599197,
    0.010834073647856712,
    0.008375768549740314,
    0.0063345106318593025,
    0.00509472144767642,
    0.004679171834141016,
    0.005019838456064463,
    0.006103371270000935,
    0.007728124503046274,
    0.009240865707397461,
    0.009823823347687721,
    0.009191756136715412,
    0.007805653382092714,
    0.006355308461934328,
    0.005221925210207701,
    0.004531474318355322,
    0.00442515267059207,
    0.00495770713314414,
    0.0058067417703568935,
    0.006305593531578779,
    0.005985396448522806,
    0.004985291976481676,
    0.0038649868220090866,
    0.003196412231773138,
    0.00318358582444489,
    0.003713807091116905,
    0.004766717553138733,
    0.0062628542073071,
    0.0077340505085885525,
    0.008663472719490528,
    0.008880174718797207,
    0.008466234430670738,
    0.007584788836538792,
    0.006524248979985714,
    0.00571235828101635,
    0.005446186754852533,
    0.005713559687137604,
    0.006346256472170353,
    0.0070848120376467705,
    0.007589127868413925,
    0.007685624063014984,
    0.0075037539936602116,
    0.007295402232557535,
    0.007413793820887804,
    0.008265870623290539,
    0.009768492542207241,
    0.011264338158071041,
    0.012201370671391487,
    0.012397395446896553,
    0.011783384718000889,
    0.010424182750284672,
    0.008693867363035679,
    0.007063154596835375,
    0.005947812460362911,
    0.005837735254317522,
    0.007086494937539101,
    0.009573225863277912,
    0.012799059972167015,
    0.016125891357660294,
    0.01891716942191124,
    0.020707188174128532,
    0.02126600407063961,
    0.02065460756421089,
    0.01923869363963604,
    0.017485182732343674,
    0.01584729552268982,
    0.014682346023619175,
    0.014055510051548481,
    0.013721109367907047,
    0.01344222854822874,
    0.013212060555815697,
    0.012996979989111423,
    0.012609930709004402,
    0.01195461768656969,
    0.011080406606197357,
    0.010151803493499756,
    0.009463333524763584,
    0.009238826110959053,
    0.009520432911813259,
    0.010208720341324806,
    0.010983077809214592,
    0.011338802985846996,
    0.01084411982446909,
    0.009435753338038921,
    0.007422406692057848,
    0.005177247803658247,
    0.003042945172637701,
    0.0013802464818581939,
    0.0004443505313247442,
    0.00028548529371619225,
    0.0007905001984909177,
    0.001723992871120572,
    0.0026642384473234415,
    0.003070677164942026,
    0.0025705862790346146,
    0.0012413020012900233,
    -0.00037140026688575745,
    -0.0016586880665272474,
    -0.0023524691350758076,
    -0.002473284490406513,
    -0.0020102618727833033,
    -0.001049747341312468,
    6.795918307034299e-05,
    0.0011908897431567311,
    0.0024646027013659477,
    0.003866195445880294,
    0.004977382719516754,
    0.0053636617958545685,
    0.004916753154247999,
    0.0038979582022875547,
    0.0027261320501565933,
    0.0017630290240049362,
    0.0012689841678366065,
    0.0011588878696784377,
    0.0011375989997759461,
    0.0010867118835449219,
    0.0012457665288820863,
    0.0017544415313750505,
    0.0024020271375775337,
    0.002993508707731962,
    0.003331162966787815,
    0.0031995035242289305,
    0.0024268741253763437,
    0.0012440717546269298,
    0.00017672250396572053,
    -0.00010922228102572262,
    0.0008808648562990129,
    0.0029785253573209047,
    0.005562886130064726,
    0.007813460193574429,
    0.009288400411605835,
    0.009847539477050304,
    0.00970027968287468,
    0.009041125886142254,
    0.008012439124286175,
    0.007063659373670816,
    0.0066971671767532825,
    0.007454952225089073,
    0.009036307223141193,
    0.010746884159743786,
    0.011838717386126518,
    0.01206098310649395,
    0.011547664180397987,
    0.010430724360048771,
    0.009188789874315262,
    0.007984699681401253,
    0.007277794647961855,
    0.007161028683185577,
    0.0076607875525951385,
    0.008345392532646656,
    0.008699812926352024,
    0.008709236979484558,
    0.008189159445464611,
    0.007481942884624004,
    0.006419939920306206,
    0.005450470373034477,
    0.004618198610842228,
    0.004015774931758642,
    0.0036534476093947887,
    0.0030398601666092873,
    0.0022802995517849922,
    0.0010631061159074306,
    0.00010453598952153698,
    -0.0008690335671417415,
    -0.0014976599486544728,
    -0.0017058877274394035,
    -0.0015272716991603374,
    -0.0009646951220929623,
    -0.0011370819993317127,
    -0.0016453503631055355,
    -0.003133164718747139,
    -0.004263777751475573,
    -0.005005305167287588,
    -0.005123641341924667,
    -0.004900801926851273,
    -0.005208145827054977,
    -0.004805391654372215,
    -0.005006733816117048,
    -0.0041604433208703995,
    -0.004601184278726578,
    -0.004883530084043741,
    -0.004003234673291445,
    -0.0027041472494602203,
    0.0002842922986019403,
    -0.0029098419472575188,
    -0.0029312940314412117
#endif
};


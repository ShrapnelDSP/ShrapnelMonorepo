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

#include <array>

static std::array fir_coeff{
    -0.0037564742378890514f,
    -0.0194765105843544f,
    -0.08748021721839905f,
    -0.26966428756713867f,
    -0.5808219313621521f,
    -0.9063171744346619f,
    -1.0386521816253662f,
    -0.8603169322013855f,
    -0.46721968054771423f,
    -0.04791257157921791f,
    0.2867630124092102f,
    0.4984447658061981f,
    0.574799656867981f,
    0.5154806971549988f,
    0.3306923508644104f,
    0.060714464634656906f,
    -0.20888927578926086f,
    -0.37036028504371643f,
    -0.3820297122001648f,
    -0.28964871168136597f,
    -0.14016802608966827f,
    0.04671409726142883f,
    0.2305663675069809f,
    0.33528295159339905f,
    0.30148687958717346f,
    0.15575389564037323f,
    -0.007246615830808878f,
    -0.1165938749909401f,
    -0.15145261585712433f,
    -0.12392398715019226f,
    -0.05786096304655075f,
    0.03507285565137863f,
    0.13526953756809235f,
    0.21170289814472198f,
    0.2343493103981018f,
    0.1867852360010147f,
    0.09546754509210587f,
    0.009756524115800858f,
    -0.03376837819814682f,
    -0.02573435753583908f,
    0.009972216561436653f,
    0.04652196541428566f,
    0.07472579181194305f,
    0.0879240334033966f,
    0.0800890326499939f,
    0.051414161920547485f,
    0.01515804510563612f,
    -0.0007666537421755493f,
    0.012387222610414028f,
    0.03294295072555542f,
    0.047237593680620193f,
    0.06432503461837769f,
    0.09276732057332993f,
    0.11616240441799164f,
    0.11090561747550964f,
    0.08753321319818497f,
    0.07502544671297073f,
    0.0789351612329483f,
    0.07730638235807419f,
    0.050708722323179245f,
    0.01286821998655796f,
    -0.008763324469327927f,
    -0.005359808448702097f,
    0.013226817362010479f,
    0.02767263725399971f,
    0.029534989967942238f,
    0.028516555204987526f,
    0.03406006097793579f,
    0.04741372913122177f,
    0.0678158849477768f,
    0.09569378942251205f,
    0.12033625692129135f,
    0.12075372040271759f,
    0.09422051906585693f,
    0.05760864540934563f,
    0.02334357425570488f,
    -0.005383335053920746f,
    -0.02656237781047821f,
    -0.03096957691013813f,
    -0.009539122693240643f,
    0.029128750786185265f,
    0.06635220348834991f,
    0.0912904366850853f,
    0.10650883615016937f,
    0.1187136247754097f,
    0.12121295183897018f,
    0.10162882506847382f,
    0.06396611034870148f,
    0.025461357086896896f,
    -0.0013506790855899453f,
    -0.016417788341641426f,
    -0.021823152899742126f,
    -0.010216502472758293f,
    0.02146880328655243f,
    0.058606214821338654f,
    0.08508080244064331f,
    0.09790182113647461f,
    0.10373619198799133f,
    0.11126091331243515f,
    0.12373356521129608f,
    0.1369648426771164f,
    0.142042338848114f,
    0.13163070380687714f,
    0.10776324570178986f,
    0.08002027124166489f,
    0.057062286883592606f,
    0.0422426238656044f,
    0.03383830934762955f,
    0.028147650882601738f,
    0.022562524303793907f,
    0.01655130460858345f,
    0.011252408847212791f,
    0.00872215535491705f,
    0.01241507101804018f,
    0.02441943995654583f,
    0.04053754732012749f,
    0.052236028015613556f,
    0.054180413484573364f,
    0.04924267902970314f,
    0.0445871539413929f,
    0.043778084218502045f,
    0.044408395886421204f,
    0.041528183966875076f,
    0.03408752754330635f,
    0.02638247422873974f,
    0.02095939964056015f,
    0.015301629900932312f,
    0.008153784088790417f,
    0.0040175351314246655f,
    0.008534149266779423f,
    0.02159642055630684f,
    0.03656883165240288f,
    0.04687534645199776f,
    0.05171499401330948f,
    0.05292370542883873f,
    0.05002389848232269f,
    0.04179229959845543f,
    0.029276829212903976f,
    0.015537804923951626f,
    0.003570778062567115f,
    -0.0047484710812568665f,
    -0.00816650502383709f,
    -0.007272896356880665f,
    -0.004781515337526798f,
    -0.0023701495956629515f,
    4.1200113628292456e-05f,
    0.0014204599428921938f,
    -0.0007130166050046682f,
    -0.007389390375465155f,
    -0.01816841959953308f,
    -0.03251173347234726f,
    -0.04821757599711418f,
    -0.06093612685799599f,
    -0.06629174202680588f,
    -0.061788614839315414f,
    -0.04751215875148773f,
    -0.026231583207845688f,
    -0.0027790414169430733f,
    0.018112724646925926f,
    0.033463168889284134f,
    0.04180353879928589f,
    0.04280052334070206f,
    0.03747948259115219f,
    0.027628561481833458f,
    0.014538433402776718f,
    -0.0008242146577686071f,
    -0.016671188175678253f,
    -0.029882842674851418f,
    -0.03644527867436409f,
    -0.03278292343020439f,
    -0.017462430521845818f,
    0.007051154039800167f,
    0.03397012874484062f,
    0.05528358370065689f,
    0.06641294062137604f,
    0.06760620325803757f,
    0.0612633042037487f,
    0.049977414309978485f,
    0.03765544295310974f,
    0.02876364439725876f,
    0.024916483089327812f,
    0.024167725816369057f,
    0.023442506790161133f,
    0.020884664729237556f,
    0.015640761703252792f,
    0.006671412847936153f,
    -0.005913218483328819f,
    -0.01907447539269924f,
    -0.028054753318428993f,
    -0.02989596128463745f,
    -0.025532154366374016f,
    -0.018342338502407074f,
    -0.01233007200062275f,
    -0.011565517634153366f,
    -0.018694384023547173f,
    -0.033133238554000854f,
    -0.05103873834013939f,
    -0.06711363792419434f,
    -0.0772828608751297f,
    -0.08035674691200256f,
    -0.07714332640171051f,
    -0.0687483623623848f,
    -0.056886106729507446f,
    -0.04503145441412926f,
    -0.037856101989746094f,
    -0.03853832185268402f,
    -0.04630838707089424f,
    -0.05750533193349838f,
    -0.06837014853954315f,
    -0.07641368359327316f,
    -0.08012834936380386f,
    -0.0781327337026596f,
    -0.0701565220952034f,
    -0.058463890105485916f,
    -0.04642091318964958f,
    -0.03612952679395676f,
    -0.027894960716366768f,
    -0.021285640075802803f,
    -0.016199957579374313f,
    -0.012791545130312443f,
    -0.011271164752542973f,
    -0.012176073156297207f,
    -0.016162855550646782f,
    -0.023242389783263206f,
    -0.03219522163271904f,
    -0.04103313013911247f,
    -0.047839898616075516f,
    -0.05131106451153755f,
    -0.05111429840326309f,
    -0.04757564514875412f,
    -0.04122688248753548f,
    -0.0329035259783268f,
    -0.02438868023455143f,
    -0.018043989315629005f,
    -0.015078673139214516f,
    -0.015089812688529491f,
    -0.016884872689843178f,
    -0.019541116431355476f,
    -0.02298358827829361f,
    -0.027195405215024948f,
    -0.03151925653219223f,
    -0.03472668677568436f,
    -0.03570104390382767f,
    -0.034775953739881516f,
    -0.033544398844242096f,
    -0.03309851512312889f,
    -0.03345469385385513f,
    -0.033738624304533005f,
    -0.03235357627272606f,
    -0.02809050865471363f,
    -0.02160836197435856f,
    -0.0156696904450655f,
    -0.013488708063960075f,
    -0.016786543652415276f,
    -0.024701224640011787f,
    -0.03375251591205597f,
    -0.03980088233947754f,
    -0.04063611850142479f,
    -0.03669653832912445f,
    -0.029845058917999268f,
    -0.022224046289920807f,
    -0.016459941864013672f,
    -0.015229298733174801f,
    -0.019251910969614983f,
    -0.026527656242251396f,
    -0.033926378935575485f,
    -0.03884386271238327f,
    -0.0398489385843277f,
    -0.03745930641889572f,
    -0.03426773101091385f,
    -0.03319589048624039f,
    -0.03587108850479126f,
    -0.04262905567884445f,
    -0.05290330573916435f,
    -0.0648411214351654f,
    -0.07552596181631088f,
    -0.0826861783862114f,
    -0.08568792790174484f,
    -0.08497437834739685f,
    -0.08131538331508636f,
    -0.07588314265012741f,
    -0.07054484635591507f,
    -0.06691592931747437f,
    -0.06498613208532333f,
    -0.0632193312048912f,
    -0.06005058065056801f,
    -0.05523053929209709f,
    -0.04990329220890999f,
    -0.04563998803496361f,
    -0.0436055064201355f,
    -0.04416409134864807f,
    -0.046566110104322433f,
    -0.04921175166964531f,
    -0.050633423030376434f,
    -0.050132445991039276f,
    -0.047294292598962784f,
    -0.04165000468492508f,
    -0.03333599492907524f,
    -0.023698054254055023f,
    -0.015099850483238697f,
    -0.009767184033989906f,
    -0.008634660392999649f,
    -0.011079149320721626f,
    -0.015326639637351036f,
    -0.019305400550365448f,
    -0.02120974287390709f,
    -0.019891390576958656f,
    -0.015641702339053154f,
    -0.010339420288801193f,
    -0.0065151541493833065f,
    -0.005972495302557945f,
    -0.008714621886610985f,
    -0.013232363387942314f,
    -0.01758558116853237f,
    -0.020117877051234245f,
    -0.019905786961317062f,
    -0.017052100971341133f,
    -0.012886131182312965f,
    -0.009465807117521763f,
    -0.0081764105707407f,
    -0.009166651405394077f,
    -0.012197955511510372f,
    -0.01720809005200863f,
    -0.02399374172091484f,
    -0.031756691634655f,
    -0.038934845477342606f,
    -0.04381569102406502f,
    -0.0455235056579113f,
    -0.04421360418200493f,
    -0.040421586483716965f,
    -0.03471613675355911f,
    -0.028103388845920563f,
    -0.02191978506743908f,
    -0.01712588593363762f,
    -0.014133544638752937f,
    -0.012922202236950397f,
    -0.013102054595947266f,
    -0.014132590964436531f,
    -0.015427269972860813f,
    -0.016258763149380684f,
    -0.0157249104231596f,
    -0.013103004544973373f,
    -0.008443421684205532f,
    -0.002616799669340253f,
    0.0029208620544523f,
    0.006572186481207609f,
    0.0075727407820522785f,
    0.006378610618412495f,
    0.004062459338456392f,
    0.0014312975108623505f,
    -0.0014373742742463946f,
    -0.004770706873387098f,
    -0.00840942095965147f,
    -0.01186327449977398f,
    -0.014839318580925465f,
    -0.017229333519935608f,
    -0.018865885213017464f,
    -0.019623536616563797f,
    -0.01959698647260666f,
    -0.019037455320358276f,
    -0.0180219653993845f,
    -0.01659233495593071f,
    -0.015209903940558434f,
    -0.014478296041488647f,
    -0.014579030685126781f,
    -0.01538778841495514f,
    -0.016952892765402794f,
    -0.019235428422689438f,
    -0.021462000906467438f,
    -0.0224313884973526f,
    -0.02134745754301548f,
    -0.018301837146282196f,
    -0.01436549797654152f,
    -0.010883968323469162f,
    -0.008562388829886913f,
    -0.007409405428916216f,
    -0.00694100558757782f,
    -0.0063561126589775085f,
    -0.005018407944589853f,
    -0.003014869522303343f,
    -0.0010012337006628513f,
    0.000773956417106092f,
    0.0025296686217188835f,
    0.004399014171212912f,
    0.006410346832126379f,
    0.00835527665913105f,
    0.010022538714110851f,
    0.01149713434278965f,
    0.012951045297086239f,
    0.014542588032782078f,
    0.01610627770423889f,
    0.01710520125925541f,
    0.017360566183924675f,
    0.017343085259199142f,
    0.0176528487354517f,
    0.01859612949192524f,
    0.020369242876768112f,
    0.02322482317686081f,
    0.026860492303967476f,
    0.030123921111226082f,
    0.031756237149238586f,
    0.0312105230987072f,
    0.029044339433312416f,
    0.026604466140270233f,
    0.025136975571513176f,
    0.02518281526863575f,
    0.026627089828252792f,
    0.028800738975405693f,
    0.03070429153740406f,
    0.0315006822347641f,
    0.030407635495066643f,
    0.026784326881170273f,
    0.02095768041908741f,
    0.014511692337691784f,
    0.009566562250256538f,
    0.007508228067308664f,
    0.008213026449084282f,
    0.010539781302213669f,
    0.013413071632385254f,
    0.016163844615221024f,
    0.018063096329569817f,
    0.01837853342294693f,
    0.017062554135918617f,
    0.01463649794459343f,
    0.01174165215343237f,
    0.009182150475680828f,
    0.007584500592201948f,
    0.0071178157813847065f,
    0.007631204556673765f,
    0.00883105956017971f,
    0.010448778979480267f,
    0.01199730858206749f,
    0.01275942474603653f,
    0.012454099021852016f,
    0.011492142453789711f,
    0.010311220772564411f,
    0.009009230881929398f,
    0.007863461039960384f,
    0.007410241756588221f,
    0.00788551103323698f,
    0.009153944440186024f,
    0.010924229398369789f,
    0.012879380024969578f,
    0.01476728543639183f,
    0.016222326084971428f,
    0.016954435035586357f,
    0.01703772507607937f,
    0.016646480187773705f,
    0.016038015484809875f,
    0.01585807278752327f,
    0.01700383424758911f,
    0.019863730296492577f,
    0.02404233068227768f,
    0.02882981300354004f,
    0.03348923847079277f,
    0.03760652244091034f,
    0.04125063866376877f,
    0.0444934219121933f,
    0.046975672245025635f,
    0.04798082262277603f,
    0.047155462205410004f,
    0.04519657790660858f,
    0.04336443170905113f,
    0.04248249530792236f,
    0.042632777243852615f,
    0.043422017246484756f,
    0.04431718960404396f,
    0.044846534729003906f,
    0.04483090713620186f,
    0.04434332996606827f,
    0.043275926262140274f,
    0.041288312524557114f,
    0.03809576854109764f,
    0.03363563120365143f,
    0.028259456157684326f,
    0.022917192429304123f,
    0.018733112141489983f,
    0.016111934557557106f,
    0.014731538482010365f,
    0.014390407130122185f,
    0.015210277400910854f,
    0.017190612852573395f,
    0.01996741257607937f,
    0.022862199693918228f,
    0.02501807175576687f,
    0.025630956515669823f,
    0.0244301650673151f,
    0.02192607894539833f,
    0.018997251987457275f,
    0.0164243932813406f,
    0.01484654936939478f,
    0.014682198874652386f,
    0.015685833990573883f,
    0.01675872504711151f,
    0.016644224524497986f,
    0.0151270916685462f,
    0.013264201581478119f,
    0.012194594368338585f,
    0.012169956229627132f,
#if 0
    0.012667597271502018f,
    0.013080847449600697f,
    0.013511157594621181f,
    0.014826846309006214f,
    0.01767025515437126f,
    0.02161911129951477f,
    0.025674525648355484f,
    0.02929791994392872f,
    0.03230809420347214f,
    0.03425689414143562f,
    0.03472146391868591f,
    0.03386275842785835f,
    0.032391343265771866f,
    0.031098617240786552f,
    0.03043729066848755f,
    0.030431952327489853f,
    0.030991330742836f,
    0.032094310969114304f,
    0.033448733389377594f,
    0.03444906696677208f,
    0.03454728052020073f,
    0.033542271703481674f,
    0.0315813384950161f,
    0.029008975252509117f,
    0.026468845084309578f,
    0.024649785831570625f,
    0.023706888779997826f,
    0.023331575095653534f,
    0.023256445303559303f,
    0.023419570177793503f,
    0.023797204717993736f,
    0.024403253570199013f,
    0.025263171643018723f,
    0.02611466869711876f,
    0.026380030438303947f,
    0.025504160672426224f,
    0.02333340048789978f,
    0.020391441881656647f,
    0.017669660970568657f,
    0.016035664826631546f,
    0.01570277474820614f,
    0.01613239385187626f,
    0.016571395099163055f,
    0.016670577228069305f,
    0.016758084297180176f,
    0.017385423183441162f,
    0.018593063578009605f,
    0.020078402012586594f,
    0.021517205983400345f,
    0.02248048037290573f,
    0.02271643839776516f,
    0.02256864868104458f,
    0.022921690717339516f,
    0.02433740720152855f,
    0.026200197637081146f,
    0.027183178812265396f,
    0.02626717835664749f,
    0.02330530621111393f,
    0.019096268340945244f,
    0.014925085939466953f,
    0.011885189451277256f,
    0.010440882295370102f,
    0.010513742454349995f,
    0.011720103211700916f,
    0.013482470065355301f,
    0.015207181684672832f,
    0.016429832205176353f,
    0.016900554299354553f,
    0.016552507877349854f,
    0.015370359644293785f,
    0.013281464576721191f,
    0.010164372622966766f,
    0.0061484286561608315f,
    0.0019366645719856024f,
    -0.001466479618102312f,
    -0.0033673469442874193f,
    -0.003552459180355072f,
    -0.0021583971101790667f,
    0.0003475575940683484f,
    0.003198848804458976f,
    0.0056252614594995975f,
    0.007060409523546696f,
    0.007120456080883741f,
    0.0057320063933730125f,
    0.0033034817315638065f,
    0.0007996452623046935f,
    -0.0007488769479095936f,
    -0.0009876807453110814f,
    -0.00017427258717361838f,
    0.0012603977229446173f,
    0.002872299635782838f,
    0.004260183311998844f,
    0.005191862117499113f,
    0.005746032111346722f,
    0.0063775572925806046f,
    0.00768424104899168f,
    0.00982810091227293f,
    0.01220938190817833f,
    0.013893883675336838f,
    0.014374190010130405f,
    0.013879808597266674f,
    0.012989381328225136f,
    0.012083880603313446f,
    0.01137850247323513f,
    0.011134369298815727f,
    0.011430439539253712f,
    0.011851547285914421f,
    0.011761197820305824f,
    0.010824644938111305f,
    0.008928338065743446f,
    0.006227536592632532f,
    0.0034251962788403034f,
    0.0014928849413990974f,
    0.0011095782974734902f,
    0.0021941643208265305f,
    0.0040814257226884365f,
    0.0060868896543979645f,
    0.007676845882087946f,
    0.008473244495689869f,
    0.008278281427919865f,
    0.006995668634772301f,
    0.00461440859362483f,
    0.0013210115721449256f,
    -0.0022626854479312897f,
    -0.0050735799595713615f,
    -0.0061197723262012005f,
    -0.004985771607607603f,
    -0.002202505711466074f,
    0.0009253703756257892f,
    0.0030096927657723427f,
    0.003092207247391343f,
    0.0011544079752638936f,
    -0.0018490190850570798f,
    -0.004573100246489048f,
    -0.006089937407523394f,
    -0.006246119271963835f,
    -0.005072025116533041f,
    -0.002710374305024743f,
    -6.013158417772502e-05f,
    0.001283355406485498f,
    0.00011211908713448793f,
    -0.0032640458084642887f,
    -0.007320340722799301f,
    -0.010680928826332092f,
    -0.01276430580765009f,
    -0.013370398432016373f,
    -0.012386827729642391f,
    -0.010405776090919971f,
    -0.009143153205513954f,
    -0.01030009612441063f,
    -0.01383979618549347f,
    -0.01796651817858219f,
    -0.021184029057621956f,
    -0.023280393332242966f,
    -0.024275321513414383f,
    -0.023882493376731873f,
    -0.021934494376182556f,
    -0.019314590841531754f,
    -0.017555493861436844f,
    -0.017288193106651306f,
    -0.018431395292282104f,
    -0.020510723814368248f,
    -0.022615764290094376f,
    -0.02374863065779209f,
    -0.02282482385635376f,
    -0.019471194595098495f,
    -0.014772249385714531f,
    -0.010701031424105167f,
    -0.008412222377955914f,
    -0.007093144580721855f,
    -0.005827171262353659f,
    -0.005256466567516327f,
    -0.005895149894058704f,
    -0.006580915302038193f,
    -0.005866005551069975f,
    -0.00393851101398468f,
    -0.00217211851850152f,
    -0.001403873786330223f,
    -0.0015668115811422467f,
    -0.002807294251397252f,
    -0.005532883573323488f,
    -0.008818492293357849f,
    -0.010523042641580105f,
    -0.009665886871516705f,
    -0.007290651090443134f,
    -0.004579862114042044f,
    -0.0017966395244002342f,
    0.0006327166920527816f,
    0.0016954891616478562f,
    0.00021967216162011027f,
    -0.004262316040694714f,
    -0.010594682767987251f,
    -0.016695521771907806f,
    -0.020722122862935066f,
    -0.021453069522976875f,
    -0.018414292484521866f,
    -0.012581798247992992f,
    -0.006965294014662504f,
    -0.005015093367546797f,
    -0.007639625109732151f,
    -0.012222744524478912f,
    -0.015374151058495045f,
    -0.015825778245925903f,
    -0.014330542646348476f,
    -0.012108058668673038f,
    -0.009963639080524445f,
    -0.00876807514578104f,
    -0.009222655557096004f,
    -0.010892442427575588f,
    -0.012706685811281204f,
    -0.014398621395230293f,
    -0.016686325892806053f,
    -0.019466014578938484f,
    -0.021540645509958267f,
    -0.02215111069381237f,
    -0.02106013335287571f,
    -0.018314369022846222f,
    -0.014688020572066307f,
    -0.011694982647895813f,
    -0.010792529210448265f,
    -0.012482999823987484f,
    -0.01590399630367756f,
    -0.01939273811876774f,
    -0.022027572616934776f,
    -0.024065371602773666f,
    -0.025735562667250633f,
    -0.026393473148345947f,
    -0.025452524423599243f,
    -0.023611528798937798f,
    -0.02207261323928833f,
    -0.021382808685302734f,
    -0.021580800414085388f,
    -0.022536905482411385f,
    -0.02387155592441559f,
    -0.02467862144112587f,
    -0.023938443511724472f,
    -0.02188674733042717f,
    -0.020200831815600395f,
    -0.020232779905200005f,
    -0.02181059867143631f,
    -0.023759840056300163f,
    -0.024907618761062622f,
    -0.02440781518816948f,
    -0.021947938948869705f,
    -0.018380828201770782f,
    -0.015306007117033005f,
    -0.013914866372942924f,
    -0.014377020299434662f,
    -0.015831993892788887f,
    -0.017094317823648453f,
    -0.017275094985961914f,
    -0.015949109569191933f,
    -0.013060144148766994f,
    -0.00891135260462761f,
    -0.004393883049488068f,
    -0.0006655515753664076f,
    0.0013842076295986772f,
    0.0013522113440558314f,
    -0.0005375618347898126f,
    -0.0034089458640664816f,
    -0.006268456112593412f,
    -0.008552984334528446f,
    -0.00980344694107771f,
    -0.009788923896849155f,
    -0.009047492407262325f,
    -0.008437230251729488f,
    -0.008501425385475159f,
    -0.009378382004797459f,
    -0.010874241590499878f,
    -0.01246032677590847f,
    -0.013575769029557705f,
    -0.013973276130855083f,
    -0.013638031668961048f,
    -0.01283829566091299f,
    -0.012193923816084862f,
    -0.012252753600478172f,
    -0.013018688187003136f,
    -0.014213775284588337f,
    -0.015633411705493927f,
    -0.016974862664937973f,
    -0.01782640814781189f,
    -0.017904605716466904f,
    -0.01737983711063862f,
    -0.01672845333814621f,
    -0.01641753315925598f,
    -0.01682618446648121f,
    -0.0175645649433136f,
    -0.017464229837059975f,
    -0.015736116096377373f,
    -0.012709013186395168f,
    -0.009568109177052975f,
    -0.007487077731639147f,
    -0.00705380504950881f,
    -0.008189843967556953f,
    -0.010601675137877464f,
    -0.014033481478691101f,
    -0.01768559217453003f,
    -0.020289376378059387f,
    -0.0210472010076046f,
    -0.019982600584626198f,
    -0.017554471269249916f,
    -0.014379601925611496f,
    -0.011261471547186375f,
    -0.00914171151816845f,
    -0.008754960261285305f,
    -0.010111899115145206f,
    -0.012401849031448364f,
    -0.014594377018511295f,
    -0.01595299504697323f,
    -0.0160529762506485f,
    -0.015047223307192326f,
    -0.013760401867330074f,
    -0.013061003759503365f,
    -0.013364626094698906f,
    -0.014479964040219784f,
    -0.015767093747854233f,
    -0.016509750857949257f,
    -0.0162662360817194f,
    -0.015117899514734745f,
    -0.013354857452213764f,
    -0.011083513498306274f,
    -0.008481999859213829f,
    -0.006120575591921806f,
    -0.004678015131503344f,
    -0.004389486741274595f,
    -0.004998540971428156f,
    -0.0060854326002299786f,
    -0.007075992878526449f,
    -0.007325080689042807f,
    -0.006630670744925737f,
    -0.005232386756688356f,
    -0.003346713026985526f,
    -0.0011623294558376074f,
    0.0009810286574065685f,
    0.0026913625188171864f,
    0.0037651825696229935f,
    0.004224757198244333f,
    0.004144548438489437f,
    0.0036510322242975235f,
    0.0029226571787148714f,
    0.0021723415702581406f,
    0.0017300753388553858f,
    0.0018389858305454254f,
    0.002596363192424178f,
    0.0040281107649207115f,
    0.00578618748113513f,
    0.0072520580142736435f,
    0.008035651408135891f,
    0.008110797964036465f,
    0.00762157654389739f,
    0.006653149146586657f,
    0.005302912555634975f,
    0.0038032885640859604f,
    0.002427150262519717f,
    0.001419589389115572f,
    0.0009085062774829566f,
    0.0008148422930389643f,
    0.000894963915925473f,
    0.0008294574217870831f,
    0.000373419898096472f,
    -0.000497916538733989f,
    -0.001567653613165021f,
    -0.002576849889010191f,
    -0.00350857968442142f,
    -0.004390930291265249f,
    -0.004981392994523048f,
    -0.005074962042272091f,
    -0.004878247156739235f,
    -0.004830504301935434f,
    -0.005123015493154526f,
    -0.005574045702815056f,
    -0.005946486722677946f,
    -0.006016282830387354f,
    -0.005530158057808876f,
    -0.004487397614866495f,
    -0.0033083679154515266f,
    -0.002619423670694232f,
    -0.0027983454056084156f,
    -0.003668748075142503f,
    -0.004681708756834269f,
    -0.0053249541670084f,
    -0.005430778954178095f,
    -0.005145102273672819f,
    -0.004561575595289469f,
    -0.0036429802421480417f,
    -0.002597747603431344f,
    -0.0019088124390691519f,
    -0.0018444585148245096f,
    -0.002151607535779476f,
    -0.002326628193259239f,
    -0.002262240508571267f,
    -0.002301550703123212f,
    -0.002525929594412446f,
    -0.00244458788074553f,
    -0.0015323222614824772f,
    0.00018250272842124104f,
    0.0023949157912284136f,
    0.00490416307002306f,
    0.007288073655217886f,
    0.008847308345139027f,
    0.009156627580523491f,
    0.008537606336176395f,
    0.00788150168955326f,
    0.007931357249617577f,
    0.00876956433057785f,
    0.010000002570450306f,
    0.011405225843191147f,
    0.013017166405916214f,
    0.014543598517775536f,
    0.015331939794123173f,
    0.014919954352080822f,
    0.01348300650715828f,
    0.011800543405115604f,
    0.010721712373197079f,
    0.010736430063843727f,
    0.011908913031220436f,
    0.013913276605308056f,
    0.01611192896962166f,
    0.017671683803200722f,
    0.01801079884171486f,
    0.017157025635242462f,
    0.01549304649233818f,
    0.013578996062278748f,
    0.012063436210155487f,
    0.011433969251811504f,
    0.011852589435875416f,
    0.013037567026913166f,
    0.014314013533294201f,
    0.014946342445909977f,
    0.014530148357152939f,
    0.013066131621599197f,
    0.010834073647856712f,
    0.008375768549740314f,
    0.0063345106318593025f,
    0.00509472144767642f,
    0.004679171834141016f,
    0.005019838456064463f,
    0.006103371270000935f,
    0.007728124503046274f,
    0.009240865707397461f,
    0.009823823347687721f,
    0.009191756136715412f,
    0.007805653382092714f,
    0.006355308461934328f,
    0.005221925210207701f,
    0.004531474318355322f,
    0.00442515267059207f,
    0.00495770713314414f,
    0.0058067417703568935f,
    0.006305593531578779f,
    0.005985396448522806f,
    0.004985291976481676f,
    0.0038649868220090866f,
    0.003196412231773138f,
    0.00318358582444489f,
    0.003713807091116905f,
    0.004766717553138733f,
    0.0062628542073071f,
    0.0077340505085885525f,
    0.008663472719490528f,
    0.008880174718797207f,
    0.008466234430670738f,
    0.007584788836538792f,
    0.006524248979985714f,
    0.00571235828101635f,
    0.005446186754852533f,
    0.005713559687137604f,
    0.006346256472170353f,
    0.0070848120376467705f,
    0.007589127868413925f,
    0.007685624063014984f,
    0.0075037539936602116f,
    0.007295402232557535f,
    0.007413793820887804f,
    0.008265870623290539f,
    0.009768492542207241f,
    0.011264338158071041f,
    0.012201370671391487f,
    0.012397395446896553f,
    0.011783384718000889f,
    0.010424182750284672f,
    0.008693867363035679f,
    0.007063154596835375f,
    0.005947812460362911f,
    0.005837735254317522f,
    0.007086494937539101f,
    0.009573225863277912f,
    0.012799059972167015f,
    0.016125891357660294f,
    0.01891716942191124f,
    0.020707188174128532f,
    0.02126600407063961f,
    0.02065460756421089f,
    0.01923869363963604f,
    0.017485182732343674f,
    0.01584729552268982f,
    0.014682346023619175f,
    0.014055510051548481f,
    0.013721109367907047f,
    0.01344222854822874f,
    0.013212060555815697f,
    0.012996979989111423f,
    0.012609930709004402f,
    0.01195461768656969f,
    0.011080406606197357f,
    0.010151803493499756f,
    0.009463333524763584f,
    0.009238826110959053f,
    0.009520432911813259f,
    0.010208720341324806f,
    0.010983077809214592f,
    0.011338802985846996f,
    0.01084411982446909f,
    0.009435753338038921f,
    0.007422406692057848f,
    0.005177247803658247f,
    0.003042945172637701f,
    0.0013802464818581939f,
    0.0004443505313247442f,
    0.00028548529371619225f,
    0.0007905001984909177f,
    0.001723992871120572f,
    0.0026642384473234415f,
    0.003070677164942026f,
    0.0025705862790346146f,
    0.0012413020012900233f,
    -0.00037140026688575745f,
    -0.0016586880665272474f,
    -0.0023524691350758076f,
    -0.002473284490406513f,
    -0.0020102618727833033f,
    -0.001049747341312468f,
    6.795918307034299e-05f,
    0.0011908897431567311f,
    0.0024646027013659477f,
    0.003866195445880294f,
    0.004977382719516754f,
    0.0053636617958545685f,
    0.004916753154247999f,
    0.0038979582022875547f,
    0.0027261320501565933f,
    0.0017630290240049362f,
    0.0012689841678366065f,
    0.0011588878696784377f,
    0.0011375989997759461f,
    0.0010867118835449219f,
    0.0012457665288820863f,
    0.0017544415313750505f,
    0.0024020271375775337f,
    0.002993508707731962f,
    0.003331162966787815f,
    0.0031995035242289305f,
    0.0024268741253763437f,
    0.0012440717546269298f,
    0.00017672250396572053f,
    -0.00010922228102572262f,
    0.0008808648562990129f,
    0.0029785253573209047f,
    0.005562886130064726f,
    0.007813460193574429f,
    0.009288400411605835f,
    0.009847539477050304f,
    0.00970027968287468f,
    0.009041125886142254f,
    0.008012439124286175f,
    0.007063659373670816f,
    0.0066971671767532825f,
    0.007454952225089073f,
    0.009036307223141193f,
    0.010746884159743786f,
    0.011838717386126518f,
    0.01206098310649395f,
    0.011547664180397987f,
    0.010430724360048771f,
    0.009188789874315262f,
    0.007984699681401253f,
    0.007277794647961855f,
    0.007161028683185577f,
    0.0076607875525951385f,
    0.008345392532646656f,
    0.008699812926352024f,
    0.008709236979484558f,
    0.008189159445464611f,
    0.007481942884624004f,
    0.006419939920306206f,
    0.005450470373034477f,
    0.004618198610842228f,
    0.004015774931758642f,
    0.0036534476093947887f,
    0.0030398601666092873f,
    0.0022802995517849922f,
    0.0010631061159074306f,
    0.00010453598952153698f,
    -0.0008690335671417415f,
    -0.0014976599486544728f,
    -0.0017058877274394035f,
    -0.0015272716991603374f,
    -0.0009646951220929623f,
    -0.0011370819993317127f,
    -0.0016453503631055355f,
    -0.003133164718747139f,
    -0.004263777751475573f,
    -0.005005305167287588f,
    -0.005123641341924667f,
    -0.004900801926851273f,
    -0.005208145827054977f,
    -0.004805391654372215f,
    -0.005006733816117048f,
    -0.0041604433208703995f,
    -0.004601184278726578f,
    -0.004883530084043741f,
    -0.004003234673291445f,
    -0.0027041472494602203f,
    0.0002842922986019403f,
    -0.0029098419472575188f,
    -0.0029312940314412117f
#endif
};


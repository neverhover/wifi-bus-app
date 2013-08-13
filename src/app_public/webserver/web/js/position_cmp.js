//2011-7-25
(function(){        //闭包
//地球半径
var _eQuatorialEarthRadius = 6378.1370;
//转换
var _d2r = (Math.PI / 180);

//转换为M
function HaversineInM(lat1,long1,lat2,long2){
    return 1000 * HaversineInKM(lat1, long1, lat2, long2);
}
//转换为KM
function HaversineInKM( lat1,  long1,  lat2,  long2){
    
    var dlong = Math.abs(long2 - long1) * _d2r;
    
    var dlat = Math.abs(lat2 - lat1) * _d2r;
    
    var a = Math.pow(Math.sin(dlat / 2), 2) + Math.cos(lat1 * _d2r) * Math.cos(lat2 * _d2r) * Math.pow(Math.sin(dlong / 2), 2);
    
    var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    
    var d = _eQuatorialEarthRadius * c;

    return d;
}
var line_sta = [],
    dis=[],
    remind_sta;     //需要提醒的站点

var sta_size = 0,   //站点数量
    sta_last,      //最近的站点索引
    dis_last,       //上一次最近的距离
    threshold;      //距离阀值,单位为米

//设置线路站点信息,使用百度的结构result[0].kn
function set_line_sta(sta_arr){
    line_sta = sta_arr;
    sta_size = line_sta.length;
}
function set_remind_sta(rm_sta){
    remind_sta = rm_sta;
}
//设置阀值
function set_reminde_threshold(v){
    if(v <= 0){
        threshold = 200;
    }else{
         threshold = v;
    }
}
//重新比较距离，并返回最近的站点对象
function rebuild_dis(gps_tmp){
    //获得所有站点到目标站点的距离集合
    dis=[];
    
    var min_tmp = 0,
        min_dis = 0,
        min_index = 0;

    //获得离这个站点最近的站点
    for(i=0;i<sta_size;i++){
        tmp_sta = line_sta[i];
        dis.push(  HaversineInM(tmp_sta.position.lat, tmp_sta.position.lng, gps_tmp.lat, gps_tmp.lng) );
    }
    min_dis = dis[0];
    //找到离此坐标最近的一个站点
    for(i=0;i<sta_size;i++){
        if(dis[i] < min_dis){
            min_dis = dis[i];
            min_index = i;
        }
       
    }
    //console.log(min_dis);
    sta_last = min_index;
    dis_last = min_dis;
    //存储此站点的index，等待与下一次坐标的距离比较,如果是下一次的更大，则是驶离,反之则是即将到达
    return min_dis;
}

function check_threshold(){
    if(remind_sta == null){
        return false;
    }
    if(line_sta[sta_last].name  == remind_sta.name){
    //表示离目标站点比较近
        if( dis_last <= threshold ){
            return true;        //表示需要提醒
        }
    }
    
    return false;
}

//检测是否到站
function check_on_station(){
    if( dis_last <= 50 ){
        return sta_last;        //表示在站
    }
    return null;
}
//检测状态,看是否是离站
function check_status(){

}
window.PosCmp = window.PosCmp || {};
PosCmp.HaversineInKM = {};
PosCmp.HaversineInM = {};
PosCmp.HaversineInKM = HaversineInKM;
PosCmp.HaversineInM = HaversineInM;

PosCmp.set_line_sta = {};
PosCmp.set_remind_sta = {};
PosCmp.set_reminde_threshold = {};
PosCmp.check_on_station = {};
PosCmp.check_threshold = {};
PosCmp.rebuild_dis = {};

PosCmp.set_line_sta = set_line_sta;
PosCmp.set_remind_sta = set_remind_sta;
PosCmp.set_reminde_threshold = set_reminde_threshold;
PosCmp.check_on_station = check_on_station;
PosCmp.check_threshold = check_threshold;
PosCmp.rebuild_dis = rebuild_dis;
})();

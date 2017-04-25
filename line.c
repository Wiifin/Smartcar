#ifndef __line__
#define __line__

typedef char uint8;
typedef short uint16;
typedef int uint32;
#define width 80
#define height 60
struct Site_t
{
    uint16 x;
    uint16 y;
};
uint8 **image_array;



/*******************************************************************/


#define _white_ 255
#define _black_ 0

#define point_max 80
#define point_cnt 60

#define search_area 8

#define dr_angle_cut 3 

struct Site_t  l[point_max], r[point_max], m[point_max];
uint16         l_cnt,        r_cnt,        m_cnt;
uint16         l_stat,       r_stat;
uint16         l_end,        r_end;


void line_scan(struct Site_t *result_l,struct Site_t *result_r, uint16 line);
uint16 line_l_scan(struct Site_t *result_l, struct Site_t start, uint16 status);
uint16 line_r_scan(struct Site_t *result_r, struct Site_t start, uint16 status);
uint16 l_sc_point();
uint16 r_sc_point();
uint16 l_extend(struct Site_t *result, uint16 l_start);
uint16 r_extend(struct Site_t *result, uint16 r_start);
uint16 l_fill();
uint16 r_fill();

void line()
{
    l_cnt = r_cnt = m_cnt = 0;
    l_stat = r_stat = 0;
    l_end = r_end = 0;

    uint16 i, j;
    for( i = height - 1 ; i >= height - 5; i -- )
    {
        line_scan( &l[l_cnt], &r[r_cnt], i );
        l_cnt++; r_cnt++;
    }

    for( ; ; )
    {

        uint16 tmp_l = 0, tmp_r = 0;
        tmp_l = line_l_scan( &l[l_cnt], l[l_cnt-1], l_stat ); 
        tmp_r = line_r_scan( &r[r_cnt], r[r_cnt-1], r_stat ); 

        //line_left
        if( -1 == tmp_l )
        {
            l_stat = l_cnt - 1;
            tmp_l = line_l_scan( &l[l_cnt], l[l_cnt-1], l_stat );
            if( -1 == tmp_l )
            {
                l_stat = -l_stat;
                tmp_l = line_l_scan( &l[l_cnt], l[l_cnt-1], l_stat );
                if( -1 == tmp_l)    {   l_end = 1;/*error，没找到点，直接到下一帧*/}
            }
        }

        l_cnt += (tmp_l == 1);
    
     
        //line_right
        if( -1 == tmp_r )
        {
            r_stat = r_cnt - 1;
            tmp_r = line_r_scan( &r[r_cnt], r[r_cnt-1], r_stat );
            if( -1 == tmp_r )
            {
                r_stat = -r_stat;
                tmp_r = line_r_scan( &r[r_cnt], r[r_cnt-1], r_stat );
                if( -1 == tmp_r )    {   r_end = 1;/*error，没找到点，直接到下一帧*/}
            }
        }

        r_cnt += (tmp_r == 1);
    
    

        //竖直扫描 ---- 弯道 or 十字
        uint16 result = 0;
        if( l_stat != 0 || r_stat != 0 )
        {
            //十字
            if( l_stat < 0 && r_stat > 0 )
            {
                tmp_l = l_fill();
                tmp_r = r_fill();
                if( tmp_l == -1 )   l_end = 1;
                if( tmp_r == -1 )   r_end = 1;
            }
        }
        //水平扫描 ---- 直道 or 环形
        else
        {



        }


        if( l_end && r_end )    break;
        if( l_cnt >= point_max && r_cnt >= point_max )  break;

    }

    for ( ; m_cnt <= l_cnt && m_cnt < r_cnt ; m_cnt ++ )
    {
        m[m_cnt].x = ( l[m_cnt].x + r[m_cnt].x ) / 2;
        m[m_cnt].y = ( l[m_cnt].y + r[m_cnt].y ) / 2;
    }

}


//普通行扫描
void line_scan(struct Site_t *result_l,struct Site_t *result_r, uint16 line)
{
    uint16 tmp_l = 0 , tmp_r = width - 1 ;
    uint16 j = 0;
    for( ; j <= width-1-j ; j++ )
    {
        if( image_array[line][j] - image_array[line][j+1] == _black_ - _white_ )
        {
            tmp_l = j + 1;
        }
        if( image_array[line][width-1-j] - image_array[line][width-2-j] == _black_ - _white_ )
        {
            tmp_r = width-j - 2;
        }
    }

    result_l->y = line;
    result_l->x = tmp_l;
    result_r->y = line;
    result_r->x = tmp_r;

}

/*
*  返回值
*      1       找到一个点，正常返回
*     -1      找不到点，正常返回
*/
uint16 line_l_scan(struct Site_t *result_l, struct Site_t start, uint16 status)
{
    uint16 last_i = start.y;
    uint16 last_j = start.x;

    if( 0 == status )
    {
        uint16 s_y, s_x0, s_x1;
        
        s_y = last_i - 1;
        if(s_y < 0) return -1;
        
        s_x0 = last_j - search_area;
        s_x1 = last_j + search_area;
        s_x0 = s_x0 < 0 ? 0 : s_x0;
        s_x1 = s_x1 > width - 1 ? width - 1 : s_x1;

        uint16 fflag = -1;

        for( ; s_x0 < s_x1; s_x0 ++ )
        {
            if( image_array[ s_y ][ s_x0 ] - image_array[ s_y ][ s_x0+1 ] == _black_ - _white_ )
            {
                uint16 tmp_1 = fflag - last_j;
                uint16 tmp_2 = s_x0 + 1 - last_j;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;
                
                if( ~fflag && tmp_1 < tmp_2 )
                {

                }
                else
                {
                    fflag = s_x0 + 1;
                }
            }
        }

        if( -1 == fflag )
        {
            //todo
            return -1;
        }
        else
        {
            result_l -> y = s_y;
            result_l -> x = fflag;
            return 1;
        }
    }



    else if( status < 0 )
    {
        uint16 s_x = last_j - 1;
        if( s_x < 0 )   return -1;

        uint16 s_y0 = last_i - search_area;
        uint16 s_y1 = last_i + search_area;
        s_y0 = s_y0 < 0 ? 0 : s_y0;
        s_y1 = s_y1 > height - 1 ? height - 1 : s_y1;

        uint16 fflag = -1;

        for( ; s_y0 < s_y1 ; s_y0++)
        {
            
            if( image_array[ s_y0 ][ s_x ] - image_array[ s_y0+1 ][ s_x ] == _white_ - _black_ )
            {
                uint16 tmp_1 = fflag - last_i; 
                uint16 tmp_2 = s_y0 - last_i;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;

                if( ~fflag && tmp_1 < tmp_2 )
                {
                }
                else
                {
                    fflag = s_y0;
                }
            }
        }

        if( -1 == fflag )
        {
            return -1;
        }
        else
        {
            result_l -> y = fflag;
            result_l -> x = s_x;
           
            return 1;
        }

        
    }



    else if( status > 0 )
    {
        uint16 s_x = last_j + 1;
        if( s_x > width - 1 )   return -1;

        uint16 s_y0 = last_i - search_area;
        uint16 s_y1 = last_i + search_area;
        s_y0 = s_y0 < 0 ? 0 : s_y0;
        s_y1 = s_y1 > height - 1 ? height - 1 : s_y1;

        uint16 fflag = -1;

        for( ; s_y0 < s_y1 ; s_y0++)
        {
            
            if( image_array[ s_y0 ][ s_x ] - image_array[ s_y0+1 ][ s_x ] == _black_ - _white_ )
            {
                uint16 tmp_1 = fflag - last_i; 
                uint16 tmp_2 = s_y0 + 1 - last_i;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;

                if( ~fflag && tmp_1 < tmp_2 )
                {
                }
                else
                {
                    fflag = s_y0 + 1;
                }
            }
        }

        if( -1 == fflag )
        {
            return -1;
        }
        else
        {
            result_l -> y = fflag;
            result_l -> x = s_x;
            return 1;
        }



    }

}

uint16 line_r_scan(struct Site_t *result_r, struct Site_t start, uint16 status)
{
    uint16 last_i = start.y;
    uint16 last_j = start.x;

    if( 0 == status )
    {
        uint16 s_y, s_x0, s_x1;
        
        s_y = last_i - 1;
        if(s_y < 0) return -1;
        
        s_x0 = last_j - search_area;
        s_x1 = last_j + search_area;
        s_x0 = s_x0 < 0 ? 0 : s_x0;
        s_x1 = s_x1 > width - 1 ? width - 1 : s_x1;

        uint16 fflag = -1;

        for( ; s_x0 < s_x1; s_x0 ++ )
        {
            if( image_array[ s_y ][ s_x0 ] - image_array[ s_y ][ s_x0+1 ] == _white_ - _black_ )
            {
                uint16 tmp_1 = fflag - last_j;
                uint16 tmp_2 = s_x0 - last_j;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;
                
                if( ~fflag && tmp_1 < tmp_2 )
                {

                }
                else
                {
                    fflag = s_x0;
                }
            }
        }

        if( -1 == fflag )
        {
            //todo
            return -1;
        }
        else
        {
            result_r -> y = s_y;
            result_r -> x = fflag;
            return 1;
        }
    }



    else if( status < 0 )
    {
        uint16 s_x = last_j - 1;
        if( s_x < 0 )   return -1;

        uint16 s_y0 = last_i - search_area;
        uint16 s_y1 = last_i + search_area;
        s_y0 = s_y0 < 0 ? 0 : s_y0;
        s_y1 = s_y1 > height - 1 ? height - 1 : s_y1;

        uint16 fflag = -1;

        for( ; s_y0 < s_y1 ; s_y0++)
        {
            
            if( image_array[ s_y0 ][ s_x ] - image_array[ s_y0+1 ][ s_x ] == _black_ - _white_ )
            {
                uint16 tmp_1 = fflag - last_i; 
                uint16 tmp_2 = s_y0 + 1 - last_i;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;

                if( ~fflag && tmp_1 < tmp_2 )
                {
                }
                else
                {
                    fflag = s_y0 + 1;
                }
            }
        }

        if( -1 == fflag )
        {
            return -1;
        }
        else
        {
            result_r -> y = fflag;
            result_r -> x = s_x;
           
            return 1;
        }

        
    }



    else if( status > 0 )
    {
        uint16 s_x = last_j + 1;
        if( s_x > width - 1 )   return -1;

        uint16 s_y0 = last_i - search_area;
        uint16 s_y1 = last_i + search_area;
        s_y0 = s_y0 < 0 ? 0 : s_y0;
        s_y1 = s_y1 > height - 1 ? height - 1 : s_y1;

        uint16 fflag = -1;

        for( ; s_y0 < s_y1 ; s_y0++)
        {
            
            if( image_array[ s_y0 ][ s_x ] - image_array[ s_y0+1 ][ s_x ] == _white_ - _black_ )
            {
                uint16 tmp_1 = fflag - last_i; 
                uint16 tmp_2 = s_y0 - last_i;
                tmp_1 = tmp_1 < 0 ? -tmp_1 : tmp_1;
                tmp_2 = tmp_2 < 0 ? -tmp_2 : tmp_2;

                if( ~fflag && tmp_1 < tmp_2 )
                {
                }
                else
                {
                    fflag = s_y0;
                }
            }
        }

        if( -1 == fflag )
        {
            return -1;
        }
        else
        {
            result_r -> y = fflag;
            result_r -> x = s_x;
            return 1;
        }



    }

}

uint16 l_sc_point()
{
    uint16 i = l_cnt;
    uint16 t1_y, t1_x;
    uint16 result = -1;
    for( ; i > 0 ; i-- )
    {
        t1_y = l[i].y;
        t1_x = l[i].x;
        if( image_array[ t1_y ][ t1_x ] - image_array[ t1_y + 1 ][ t1_x ] == _white_ - _black_ )  continue;
        else
        {
            result = i;
            break;
        } 
    }
    return result;
}


uint16 r_sc_point()
{
    uint16 i = r_cnt;
    uint16 t1_y, t1_x;
    uint16 result = -1;
    for( ; i > 0 ; i-- )
    {
        t1_y = r[i].y;
        t1_x = r[i].x;
        if( image_array[ t1_y ][ t1_x ] - image_array[ t1_y + 1 ][ t1_x ] == _white_ - _black_ )  continue;
        else
        {
            result = i;
            break;
        } 
    }
    return result;
}

uint16 l_extend(struct Site_t *result, uint16 l_start)
{
    uint16 x_minus = l[l_start].x - l[l_start-2].x;
    uint16 y_minus = l[l_start].y - l[l_start-2].y;
    y_minus = y_minus == 0 ? 1 : y_minus;
    
    uint16 x_dis = x_minus / -y_minus;
    uint16 y_dis = 1;

    uint16 x_start = l[l_start].x + x_dis;
    uint16 y_start = l[l_start].y - 1;

    uint16 white_flag = -1;
    uint16 fflag = -1;
    
    for( ; y_start >= 0 ; y_start -- )
    {
        uint16 x0 = x_start - search_area;
        uint16 x1 = x_start + search_area;
        x0 = x0 < 0 ? 0 : x0;
        x1 = x1 >= width ? width - 1 : x1;

        fflag = -1;

        for( ; x0 < x1 ; x0 ++ )
        {
            if( image_array[y_start][x0] - image_array[y_start][ x0+1 ] == _black_ - _white_ )
            {
                fflag = x0 + 1;
            }
        }

        if( fflag == -1 )   white_flag = 1;
        else if( white_flag != -1 )  break;

        x_start += x_dis;
    }

    if( white_flag == 1 && fflag != -1 )
    {
        result->y = y_start;
        result->x = fflag;
        return 1;
    }
    else
    {
        return -1;
    }
}

uint16 r_extend(struct Site_t *result, uint16 r_start)
{
    uint16 x_minus = r[r_start].x - r[r_start-2].x;
    uint16 y_minus = r[r_start].y - r[r_start-2].y;
    y_minus = y_minus == 0 ? 1 : y_minus;
    
    uint16 x_dis = x_minus / -y_minus;
    uint16 y_dis = 1;

    uint16 x_start = r[r_start].x + x_dis;
    uint16 y_start = r[r_start].y - 1;

    uint16 white_flag = -1;
    uint16 fflag = -1;
    
    for( ; y_start >= 0 ; y_start -- )
    {
        uint16 x0 = x_start - search_area;
        uint16 x1 = x_start + search_area;
        x0 = x0 < 0 ? 0 : x0;
        x1 = x1 >= width ? width - 1 : x1;

        fflag = -1;

        for( ; x0 < x1 ; x0 ++ )
        {
            if( image_array[y_start][x0] - image_array[y_start][ x0+1 ] == _white_ - _black_ )
            {
                fflag = x0;
            }
        }

        if( fflag == -1 )   white_flag = 1;
        else if( white_flag != -1 )  break;

        x_start += x_dis;
    }

    if( white_flag == 1 && fflag != -1 )
    {
        result->y = y_start;
        result->x = fflag;
        return 1;
    }
    else
    {
        return -1;
    }
}


uint16 l_fill()
{
    uint16 l_cnt_temp = l_sc_point();
    if( l_cnt_temp == -1 )  return -1;
    l_cnt = l_cnt_temp;
    
    struct Site_t l_point;
    uint16 l_result = l_extend( &l_point , l_cnt );
    if( l_result == -1 )    return -1;

    uint16 x_minus = l_point.x - l[l_cnt].x;
    uint16 y_minus = l[l_cnt].y - l_point.y;
    uint16 x_dis = x_minus / y_minus;
    uint16 y_start = l[l_cnt].y - 1;
    uint16 y_end = l_point.y;
    uint16 x_start = l[l_cnt].x + x_dis;
    l_cnt ++ ;

    for ( ; y_start >= y_end && l_cnt < point_max; y_start -- )
    {
        l[l_cnt].x = x_start;
        l[l_cnt].y = y_start;
        if( y_start == y_end )  l[l_cnt].x = l_point.x;
        l_cnt++;
    }

    return 1;
}


uint16 r_fill()
{
    uint16 r_cnt_temp = r_sc_point();
    if( r_cnt_temp == -1 )  return -1;
    r_cnt = r_cnt_temp;
    
    struct Site_t r_point;
    uint16 r_result = r_extend( &r_point , r_cnt );
    if( r_result == -1 )    return -1;

    uint16 x_minus = r_point.x - r[r_cnt].x;
    uint16 y_minus = r[r_cnt].y - r_point.y;
    uint16 x_dis = x_minus / y_minus;
    uint16 y_start = r[r_cnt].y - 1;
    uint16 y_end = r_point.y;
    uint16 x_start = r[r_cnt].x + x_dis;
    r_cnt ++ ;

    for ( ; y_start >= y_end && r_cnt < point_max; y_start -- )
    {
        r[r_cnt].x = x_start;
        r[r_cnt].y = y_start;
        if( y_start == y_end )  r[r_cnt].x = r_point.x;
        r_cnt++;
    }

    return 1;
}

#endif
select * from (
  select
    id_gp_dict,
    cod_good,
    num_value,
    text_value,
    date_value,
    ora_id_num,
    id_doc,
    date_begin,
    date_end,
    disp_value,
    ora_s_time,
    oper_log
  from reappraisal.gp_values_log t where t.ora_s_time > 5431274
  order by ora_s_time
)
where rownum < 10

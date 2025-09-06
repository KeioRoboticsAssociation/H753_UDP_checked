# H753_UDP_checked
# 注意!  
## ~~コードを生成するたびにip4.cの515行目と516行目を以下のコードと置き換えてください。hardFaultになります~~
# 修正しました！もう置き換える必要はありません！
ip_addr_copy_from_ip4(ip_data.current_iphdr_dest, iphdr->dest);  
ip_addr_copy_from_ip4(ip_data.current_iphdr_src, iphdr->src);  
上記を削除


// --- ここから変更 ---  
  // HardFaultを完全に回避するため、コンパイラに最適化させない1バイトずつのコピーを行う  
  u8_t *src_ptr;  
  u8_t *dest_ptr;  

  // 宛先アドレスのコピー  
  src_ptr = (u8_t*)&(iphdr->dest);  
  dest_ptr = (u8_t*)&(ip_data.current_iphdr_dest);  
  dest_ptr[0] = src_ptr[0];  
  dest_ptr[1] = src_ptr[1];  
  dest_ptr[2] = src_ptr[2];  
  dest_ptr[3] = src_ptr[3];  

  // 送信元アドレスのコピー  
  src_ptr = (u8_t*)&(iphdr->src);  
  dest_ptr = (u8_t*)&(ip_data.current_iphdr_src);  
  dest_ptr[0] = src_ptr[0];  
  dest_ptr[1] = src_ptr[1];  
  dest_ptr[2] = src_ptr[2];  
  dest_ptr[3] = src_ptr[3];  
  // --- ここまで変更 ---


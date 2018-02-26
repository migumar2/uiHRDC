
skippingFTBIT.post.convX(1)=2;
skippingFTBIT.post.convY(1)=10.000000;
skippingFTBIT.post.convavgX(1)=2;
skippingFTBIT.post.convavgY(1)=10.000000;
figure(1)
hold on;
 plot(skippingFTBIT.post.convX,skippingFTBIT.post.convY,'*k');
 plot(skippingFTBIT.post.convavgX,skippingFTBIT.post.convavgY,'.-k');
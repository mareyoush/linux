#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/netlink.h>
#include <linux/in6.h>
#include <linux/list.h>
#include <linux/if.h>


#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb)
{
        struct nlmsghdr *nlh;
        int pid;
        struct sk_buff *skb_out;
        int msg_size;
       	char *msg = "Hello from FAZA";
    	int res;
	char *lol;

	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

     	 msg_size = strlen(msg);
		
         nlh = (struct nlmsghdr *)skb->data;
/// moje testy
	lol=(char *)nlmsg_data(nlh);
	if (lol[0]=='1')
		printk(KERN_INFO "jest ok");
	else
		printk(KERN_INFO "jest nie ok %c", lol[0]);

		/*
		struct net_device *dev = __dev_get_by_name(0,"eth0");
		dev->dev_addr; // is the MAC address
		*/
struct net_device *dev;
struct in_device* in_dev;
struct in_ifaddr* if_info;


//struct inet6_dev drugi;

dev = first_net_device(&init_net);
in_dev=(struct in_device *)dev->ip_ptr;
if_info = in_dev->ifa_list;
	printk(KERN_INFO"\nadres %x",if_info->ifa_address);
	printk(KERN_INFO"\nmask %x",if_info->ifa_mask);	
//drugi=dev->ip6_ptr;
	//if (drugi==NULL)
	//printk("\n %s",drugi);
	//else
	//printk("\n pelny");
	

  //device = dev_get_by_name("eth0");

//while (dev) {
    printk(KERN_INFO "found [%02X]\n", dev->dev_addr[5]);

//"%02X%c", dev->dev_addr[i], (i<5)
    //dev = next_net_device(dev);
//}

	printk(KERN_INFO "%s",dev->dev_addr);


// ipv6
struct in6_addr *if6addr;
struct inet6_ifaddr * if6ap;
struct inet6_dev * in6_dev;
in6_dev=rcu_dereference(dev->ip6_ptr);
if (in6_dev==NULL )
	printk(KERN_INFO "\n nie ma ipv6");
else
{
	if6ap=&in6_dev->addr_list;
	if6addr=if6ap->addr;
	printk(KERN_INFO"\n ipv6 %s:",if6addr->in6_u);
}

////
   	printk(KERN_INFO "Netlink received msg payload:%s\n", (char *)nlmsg_data(nlh));
   	pid = nlh->nlmsg_pid; /*pid of sending process */

     	skb_out = nlmsg_new(msg_size, 0);
      	 if (!skb_out) {
  	 printk(KERN_ERR "Failed to allocate new skb\n");
        return;
   }

       nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
     NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
      strncpy(nlmsg_data(nlh), msg, msg_size);

          res = nlmsg_unicast(nl_sk, skb_out, pid);
        if (res < 0)
  	 printk(KERN_INFO "Error while sending bak to user\n");
}

struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
};

static int __init hello_init(void)
{
        printk("Entering: %s\n", __FUNCTION__);
        nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
       	if (!nl_sk) {
    		printk(KERN_ALERT "Error creating socket.\n");
     	return -10;
  	  }
return 0;
}

static void __exit hello_exit(void)
{
        printk(KERN_INFO "exiting hello module\n");
            netlink_kernel_release(nl_sk);
}

module_init(hello_init); module_exit(hello_exit);

MODULE_LICENSE("GPL");

# UE5BlasterLearning

```cpp
//UFUNCTION(NetMulticast, Reliable)
//void MulticastAmmo(int32 UpdateAmmo);

UFUNCTION(Client, Reliable)
void ClientSyncAmmo(int32 UpdateAmmo);

/*
*我之前修改武器弹药异常这个Bug的时候使用的是上面的多播RPC去纠正客户端的武器弹药，但是实际上真正需要纠正弹药的只有玩家控制的这台客户端，
*所以这条多播RPC可以进行优化，没必要再去发送到其余的客户端造成无意义的网络资源耗费。
*所以我修改多播RPC为指定客户端RPC，在装备时服务器只会向玩家控制的这台客户端进行同步纠正弹药。
*但是注意这一步需要在设置完武器的所有者之后执行，也就是修改Combatponent.cpp的代码如下。删除装备武器方法当中的多播RPC调用。在装备主副武器的方法中添加指定客户端RPC的调用。
*/

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccuiped) return;
        //WeaponToEquip->MulticastAmmo(WeaponToEquip->GetAmmo());
  ...
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(WeaponToEquip);
	PlayEquipWeaponSound(WeaponToEquip);
	SecondaryWeapon->SetOwner(Character);
	WeaponToEquip->ClientSyncAmmo(WeaponToEquip->GetAmmo());
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRightHand(EquippedWeapon);
	EquippedWeapon->SetOwner(Character);
	WeaponToEquip->ClientSyncAmmo(WeaponToEquip->GetAmmo());
	...
}

//如果有人在后续的测试的时候发现，我敲这个爆头伤害的判定不对啊，怎么打他后背也是爆头，打他侧面也是爆头。反正不从正面打他都是爆头，难不成这个游戏还有隐藏的偷袭机制。
//没错这都被你发现了。 狗头
//其实实际是因为，作者在设计的时候，倒带判断伤害的时候，先计算的头部HitBox，把其他部位的HitBox设置无碰撞，这就导致其实实际上的判定是穿过了外部的HitBox直接打中头部了。
//解决办法有两种，一种是修改头部HitBox的大小不和其他的重叠（诶，怎么头顶尖尖的）。还有一种是修改代码，把判定顺序修改一下，先判定其他部位再判定头部，这样实际打中其他部位的子弹就不会再计算为爆头了。
//你怎么不像上面似的告诉我咋改啊？因为本菜鸟也是一个初学者，还在苦思冥想我的爆破模式，Blaster的意思也可以翻译为爆破，可这个游戏只有个人、夺旗、团队三种模式。
//这不就相当于老婆饼里没有老婆，海参炒面没有海参吗？所以我正在设想自己的爆破模式，也是为了制作自己的功能和想法来更好的找工作。我已经实现完了爆破模式的基本功能，再加上一个角色死亡切换到剩余队友视角还没想好怎么实现。
//不过我毕竟是只小菜鸡，所以实现的功能还是比较菜的没有这个作者这么好。像更高阶的背包通过赚的钱开局买武器之类的功能还在想要不要加，因为还得学习不知道时间够不够。毕竟已经在家学习小半年了。再呆呆都快返祖成山顶洞人了。得准备准备开始找工作了。
//没想到我这么啰嗦你都能看到这里，失敬失敬。祝愿你之后一路顺风，写的代码完美运行效率高效Bug不存，家庭和睦身体健康万事如意。
//我也对整个项目进行了整理，蓝图和代码都已经整理了文档，通过我自己的理解解释了所有的功能，不过还没有结构化的归类，如果后续有机会整理归类完毕也会更新到GitHub之上。谢谢你听我的牢骚。鲜花 鲜花
```

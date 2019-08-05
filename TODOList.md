# 模型

## 1. 最小化总超载量

### Notation

| Set  | Description      | Size | Element | Remark |
| ---- | ---------------- | ---- | ------- | ------ |
| $N$  | nodes            |      |         |        |
| $L$  | undirected links |      |         |        |
| $T$  | traffics         |      |         |        |

### Constant

| Constant      | Description                                                  | Type | Remark |
| ------------- | ------------------------------------------------------------ | ---- | ------ |
| $\Lambda_i^l$ | $\Lambda_i^l =1$ when node $i$ is the  origin node of link $l$  <br />$\Lambda_i^l =-1$ when node $i$ is the  terminal node of link $l$<br />$\Lambda_i^l =0$  otherwise. |      |        |
| $T_i^t$       | $T_i^t =1$ when node $i$ is the  origin node of  traffic $t$<br />$T_i^t =-1$ when node $i$ is the  terminal node of traffic $t$<br />$T_i^t =0$  otherwise. |      |        |

### Decision

| Variable | Description                                                  | Type   | Remark |
| -------- | ------------------------------------------------------------ | ------ | ------ |
| $v_l^t$  | equals 1 if traffic $t$ uses $l$ in the forward direction, and 0 otherwise | binary |        |
| $w_l^t$  | equals 1 if traffic $t$ uses $l$ in the backward direction, and 0 otherwise | binary |        |

### Constraint

(1) flow constraint
$$
\sum_{l=1}^{|L|} (\Lambda_i^lv_l^t-\Lambda_i^lw_l^t)= T_i^t, \forall i\in N,t \in T
$$
(2) overload constraint
$$
\sum_{t=1}^{|T|}v_l^t - c_l \le 1
$$

$$
\sum_{t=1}^{|T|}w_l^t - d_l \le 1
$$

$$
c_l \ge 0,d_l \ge 0, \forall l \in L
$$

### Objective

$$
min \quad F = \sum_{l=1}^{|L|}c_l+d_l
$$

## **2. 最小化总冲突业务数** （待实现）

Notation 和 Constant 同上
### Decision

| Variable | Description                                                  | Type   | Remark |
| -------- | ------------------------------------------------------------ | ------ | ------ |
| $v_l^t$  | equals 1 if traffic $t$ uses $l$ in the forward direction, and 0 otherwise | binary |        |
| $w_l^t$  | equals 1 if traffic $t$ uses $l$ in the backward direction, and 0 otherwise | binary |        |
| $x_t$    | equals 1 if traffic $t$ belongs to the conflict traffics set | binary |        |
| $y_l$    | auxiliary variable                                           | binary |        |

### Constraint

(1) Flow constraint
$$
\sum_{l=1}^{|L|} (\Lambda_i^lv_l^t-\Lambda_i^lw_l^t)= T_i^t, \forall i\in N,t \in T
$$

(2) Record the conflict traffics: If $\sum_{t=1}^{|T|}v_l^t \ge 2$ and $v_l^t = 1$, then  $x_t =1$ (It's the same for $w_l^t$).  Let $y_l =1$ only if $\sum_{t=1}^{|T|}v_l^t \ge 2$.
$$
\sum_{t=1}^{|T|}v_l^t \ge 2 +M(y_l-1), \forall l \in L
$$

$$
\sum_{t=1}^{|T|}v_l^t < 2+My_l, \forall l \in L
$$

$$
x_t \ge y_l+v_l^t-1, \forall l\in L, t \in T
$$

### Objective

$$
min\quad \sum_{t=1}^{|T|}x_t
$$

# 策略

1. **初始解构造**

   | 策略                                                         | 已完成？ |
   | ------------------------------------------------------------ | -------- |
   | 贪心构造                                                     | O        |
   | 对所有业务求解最短路径，然后以该最短路径进行构造业务冲突表，再使用图着色进行颜色分配 | O        |
   | 设置所有边的容量为颜色数，使用多货物流模型对所有业务进行路径求解，再使用图着色进行颜色分配 |          |

2. **局部搜索**

   邻域动作定义为变动某个冲突业务的颜色：从所有的冲突业务中随机挑选一个业务进行颜色的更改。

   | 策略                                                         | 已完成？ |
   | ------------------------------------------------------------ | -------- |
   | 对于被选定的业务，使用**dijkstra求解最小超载量路径**对其它颜色盒子进行**近似评估**，做动作时该业务更改到相应的最佳颜色盒子同时**路径使用之前求解的最小超载量路径**，迭代一定次数之后使用模型对**所有超载量大于0的颜色盒子**进行最小化超载量处理。 | O        |
   | 对于被选定的业务，使用**dijkstra求解最小超载量路径**对其它颜色盒子进行**近似评估**，做动作时采用**完整模型**进行路径更改。 | O        |
   | 使用**完整模型**进行精确评估                                 | O        |
   | 使用**线性松弛模型**进行近似评估，做动作时采用**完整模型**进行路径更改 | O        |
   | 使用拉格朗日松弛对模型进行优化                               |          |
   | 使用列生成对模型进行优化                                     |          |

### 其它

1. 判定版本：给定总超载量及业务，判断能否在该超载量下将所有业务部署到网络
2. （*求出的解很可能不是最优解*）限制链路不能超载，使用模型最大化可容纳的业务数 ，剩余的业务使用最小化超载量（冲突业务数）进行计算，即可得到总超载量（总冲突业务数）
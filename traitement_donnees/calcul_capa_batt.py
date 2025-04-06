import matplotlib.pyplot as plt

consoA = 125
print(consoA)
consoE = 0.8+3.6*3+0.27


totalConso1 = 53000
totalConso2 = 43000
totalConso3 = 63000
totalConso4 = 73000

durer_envoi = 20
print(durer_envoi)

temps_att_min = []
jours_batterie = []
jours_batterie2 = []
jours_batterie3 = []
jours_batterie4 = []

for i in range(1,90):
    print(i)
    temps_att_min.append(i)
    nbparh = 60/i

    conso_A_h = ((durer_envoi*nbparh)/3600)*consoA 
    conso_E_h =  ((3600-(durer_envoi*nbparh))/3600)*consoE



    total_conso_h = conso_A_h + conso_E_h

    nb_h_batterie = totalConso1/total_conso_h 
    nb_J_batterie =nb_h_batterie /24
    #print(total_conso_h)

    #print(nb_h_batterie)
    jours_batterie.append(nb_J_batterie)
    print(nb_J_batterie)
temps_att_min = []
for i in range(1,90):
    print(i)
    temps_att_min.append(i)
    nbparh = 60/i

    conso_A_h = ((durer_envoi*nbparh)/3600)*consoA 
    conso_E_h =  ((3600-(durer_envoi*nbparh))/3600)*consoE



    total_conso_h = conso_A_h + conso_E_h

    nb_h_batterie = totalConso2/total_conso_h 
    nb_J_batterie2 =nb_h_batterie /24
    #print(total_conso_h)

    #print(nb_h_batterie)
    jours_batterie2.append(nb_J_batterie2)
    print(nb_J_batterie)
temps_att_min = []
for i in range(1,90):
    print(i)
    temps_att_min.append(i)
    nbparh = 60/i

    conso_A_h = ((durer_envoi*nbparh)/3600)*consoA 
    conso_E_h =  ((3600-(durer_envoi*nbparh))/3600)*consoE



    total_conso_h = conso_A_h + conso_E_h

    nb_h_batterie = totalConso3/total_conso_h 
    nb_J_batterie3 =nb_h_batterie /24
    #print(total_conso_h)

    #print(nb_h_batterie)
    jours_batterie3.append(nb_J_batterie3)
    print(nb_J_batterie)

temps_att_min = []
for i in range(1,90):
    print(i)
    temps_att_min.append(i)
    nbparh = 60/i

    conso_A_h = ((durer_envoi*nbparh)/3600)*consoA 
    conso_E_h =  ((3600-(durer_envoi*nbparh))/3600)*consoE



    total_conso_h = conso_A_h + conso_E_h

    nb_h_batterie = totalConso4/total_conso_h 
    nb_J_batterie4 =nb_h_batterie /24
    #print(total_conso_h)

    #print(nb_h_batterie)
    jours_batterie4.append(nb_J_batterie4)
    print(nb_J_batterie)

print(len(temps_att_min))
print(len(jours_batterie))
print(consoE)

plt.plot(temps_att_min, jours_batterie4, label='73000 mAh')
plt.plot(temps_att_min, jours_batterie3, label='63000 mAh')
plt.plot(temps_att_min, jours_batterie, label='53000 mAh')
plt.plot(temps_att_min, jours_batterie2, label='43000 mAh')




plt.ylabel('Intervalle entre deux transmissions (minutes)') 
plt.xlabel('Autonomie de la batterie (jours)')
plt.title("Impact de l'intervalle de transmission sur l'autonomie de l'appareil")
plt.legend()
plt.show()